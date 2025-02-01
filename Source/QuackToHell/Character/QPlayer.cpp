// Copyright_Team_AriAri


#include "Character/QPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Character/QNPC.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"

AQPlayer::AQPlayer()
	:Super(), CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)), // delegate와 콜백 함수 바인딩
				FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
				JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	/*캡슐 콜라이더 산하 컴포넌트*/
	RootComponent= this->GetCapsuleComponent();
	//충돌처리
	InteractionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphereComponent->SetupAttachment(RootComponent);
	InteractionSphereComponent->SetSphereRadius(SphereRadius);
	InteractionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AQPlayer::OnOverlapBegin);
	InteractionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &AQPlayer::OnOverlapEnd);
	//스프링암
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 460.f;
	

	/*스프링암 산하 컴포넌트*/
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	/*회전값 세팅*/
	this->bUseControllerRotationYaw = false;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritRoll = false;
	this->GetCharacterMovement()->bOrientRotationToMovement=true;
	/*위치값 세팅*/
	this->GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -18.f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
	/*캡슐 콜라이더 세팅*/
	this->GetCapsuleComponent()->InitCapsuleSize(21.0f, 21.0f);

	// lobby test
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("/Game/Blueprints/UI/Lobby/WBP_StartLevel"));
	if (WidgetFinder.Succeeded())
	{
		StartLevelWidget = WidgetFinder.Class;
	}
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue,
				FString::Printf(TEXT("Found Subsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
		}
	}
}

TObjectPtr<AActor> AQPlayer::GetClosestNPC()
{
	if (OverlappingNPCs.Num() == 0) {
		//대화 대상 없음
		return nullptr;
	}

	TObjectPtr<AActor> ClosestNPC = nullptr;
	float MinDistance = FLT_MAX;

	for (TObjectPtr<AActor> NPC : OverlappingNPCs) {
		//플레이어와 NPC간 거리
		float Distance = FVector::Dist(this->GetActorLocation(), NPC->GetActorLocation());
		//최소거리찾기
		if (Distance < MinDistance) {
			MinDistance = Distance;
			ClosestNPC = NPC;
		}
	}
	return ClosestNPC;
}

void AQPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (!StartLevelWidget) return;
	UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), StartLevelWidget);
	if (WidgetInstance)
	{
		WidgetInstance->AddToViewport();
	}
}

void AQPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//캐스팅 미성공시 nullptr
	TObjectPtr<AQNPC> OpponentNPC = Cast<AQNPC>(OtherActor);
	if (OpponentNPC) {
		OverlappingNPCs.Add(OtherActor);
	}
}

void AQPlayer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//캐스팅 미성공시 nullptr
	TObjectPtr<AQNPC> OpponentNPC = Cast<AQNPC>(OtherActor);
	if (OpponentNPC) {
		OverlappingNPCs.Remove(OtherActor);
	}
}

// -------------------------------------------------------------------------------------------------------- //
void AQPlayer::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// replicate할 프로퍼티 등록
	DOREPLIFETIME(AQPlayer, bCanStartConversP2N);
	DOREPLIFETIME(AQPlayer, bCanFinishConversP2N);
}

void AQPlayer::ServerRPCCanStartConversP2N_Implementation(const AQNPC* NPC)
{
}

void AQPlayer::ServerRPCCanFinishConversP2N_Implementation(const AQNPC* NPC)
{
}

void AQPlayer::ServerRPCStartConversation_Implementation(const AQNPC* NPC)
{
}

void AQPlayer::ServerRPCFinishConversation_Implementation(const AQNPC* NPC)
{
}

// ------------------------------------------------------------------------------------------------- //

bool AQPlayer::GetCanStartConversP2N(const AQNPC* NPC)
{
	ServerRPCCanStartConversP2N_Implementation(NPC);
	return bCanStartConversP2N;
}

bool AQPlayer::GetCanFinishConversP2N(const AQNPC* NPC)
{
	ServerRPCCanFinishConversP2N_Implementation(NPC);
	return bCanFinishConversP2N;
}

void AQPlayer::StartConversation(const AQNPC* NPC)
{
}

void AQPlayer::FinishConversation(const AQNPC* NPC)
{
}

void AQPlayer::CreateSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}
	// NAME_GameSession : 전역변수
	auto ExistingSession = OnlineSessionInterface->GetNamedSession(*NewSessionName);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(*NewSessionName);
	}

	// session interface의 델리게이트 리스트의 우리가 만든 델리게이트 추가
	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// CreateSession : 인자 -> const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = false;                     // LAN 여부
	SessionSettings->NumPublicConnections = 8;                // 최대 플레이어 수
	SessionSettings->bAllowJoinInProgress = true;             // 진행 중 세션 참여 허용
	SessionSettings->bAllowJoinViaPresence = true;            // Presence를 통한 참여 허용
	SessionSettings->bShouldAdvertise = true;                 // 세션 공개 여부
	SessionSettings->bUsesPresence = true;                    // Presence 사용
	SessionSettings->bUseLobbiesIfAvailable = true;           // 로비 사용 가능

	// setting session name 
	SessionSettings->Set(FName(TEXT("SessionName")), NewSessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName(TEXT("Map")), FString(TEXT("Forest")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	FString HostName = TEXT("Unknown");

	// LocalPlayer가 존재하고, Online Identity가 있으면 닉네임 가져오기
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Identity.IsValid() && LocalPlayer->GetPreferredUniqueNetId().IsValid())
		{
			HostName = Identity->GetPlayerNickname(*LocalPlayer->GetPreferredUniqueNetId());
		}
	}

	SessionSettings->Set(FName(TEXT("HostName")), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), *NewSessionName, *SessionSettings);
}

void AQPlayer::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			FString SessionNameString = SessionName.ToString();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue,
				FString::Printf(TEXT("Created session: %s"), *SessionNameString));
		}

		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString("/Game/Maps/Lobby?listen"));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
				FString::Printf(TEXT("Failed to create session!")));
		}
	}
}

void AQPlayer::JoinSession()
{
	if (SessionSearch.IsValid() && SessionSearch->SearchState == EOnlineAsyncTaskState::InProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 검색 요청 중입니다. 새로운 요청을 무시합니다."));
		return;
	}

	// delegate 등록
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	
	// Find game session
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->bIsLanQuery = false;
	// 검색 조건 추가
	//SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(FName(TEXT("SessionName")), NewSessionName, EOnlineComparisonOp::Equals);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void AQPlayer::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red,
				FString::Printf(TEXT("Can not Find Session")));
		}
		return;
	}
	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString OwningUser = Result.Session.OwningUserName;
		int32 MaxConnectionNum = Result.Session.SessionSettings.NumPublicConnections;
		int32 CurrentConnectionNum = Result.Session.NumOpenPublicConnections;
		// SessionName 가져오기
		FString SessionName;
		Result.Session.SessionSettings.Get(FName("SessionName"), SessionName);

		/** @todo 조건에 따라 리스트에 보여줄 세션 거르기 */

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue,
				FString::Printf(TEXT("Found Session - id : %s host : %s \n Session Name : %s \n player num : %d/%d"),
					*Id, *OwningUser, *SessionName, CurrentConnectionNum, MaxConnectionNum));
		}

		// Join 관련 delegate 등록
		if (!OnlineSessionInterface.IsValid()) return;
		OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

		// JoinSession
		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), *SessionName, Result);
	}
}

void AQPlayer::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid()) return;

	// Returns the platform specific connection information for joining the match
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow,
				FString::Printf(TEXT("connect: %s"), *Address));
		}

		TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, TRAVEL_Absolute);
		}
	}
}
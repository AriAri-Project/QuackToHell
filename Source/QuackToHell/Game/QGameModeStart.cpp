// Copyright_Team_AriAri


#include "Game/QGameModeStart.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "QStartPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "UObject/ConstructorHelpers.h"

AQGameModeStart::AQGameModeStart()
:CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)), // delegate와 콜백 함수 바인딩
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	PlayerControllerClass = AQStartPlayerController::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(TEXT("/Game/Blueprints/Character/BP_StartPlayer.BP_StartPlayer_C"));
	if (PawnClassRef.Class)
	{
		DefaultPawnClass = PawnClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("/Game/Blueprints/UI/Lobby/WBP_StartLevel"));
	if (WidgetFinder.Succeeded())
	{
		StartLevelWidget = WidgetFinder.Class;
	}
}

void AQGameModeStart::BeginPlay()
{
	Super::BeginPlay();

	// lobby test
	if (StartLevelWidget)
	{
		UUserWidget* StartWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), StartLevelWidget);
		if (StartWidget)
		{
			// 위젯을 화면에 추가
			StartWidget->AddToViewport();
		}
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

void AQGameModeStart::CreateSession()
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

void AQGameModeStart::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
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

void AQGameModeStart::FindSession()
{
}

void AQGameModeStart::OnFindSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void AQGameModeStart::JoinSession()
{
	if (SessionSearch.IsValid() && SessionSearch->SearchState == EOnlineAsyncTaskState::InProgress)
	{
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

void AQGameModeStart::OnFindSessionComplete(bool bWasSuccessful)
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

void AQGameModeStart::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid()) return;

	// Returns the platform specific connection information for joining the match
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, TRAVEL_Absolute);
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow,
				FString::Printf(TEXT("connect: %s"), *Address));
		}
	}
}
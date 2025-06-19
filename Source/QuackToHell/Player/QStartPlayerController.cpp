// Copyright_Team_AriAri


#include "Player/QStartPlayerController.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"
#include "QPlayerState.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "UObject/ConstructorHelpers.h"

AQStartPlayerController::AQStartPlayerController()
:	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)), // delegate와 콜백 함수 바인딩
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))

{
	bShowMouseCursor = true;
	bActorSeamlessTraveled = true;
}

void AQStartPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld(), STEAM_SUBSYSTEM);;
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

void AQStartPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	
	OnlineIdentity = Online::GetIdentityInterface(GetWorld(), STEAM_SUBSYSTEM);
	if (OnlineIdentity.IsValid())
	{
		PlayerNickName = OnlineIdentity->GetPlayerNickname(0);
		GetPlayerState<AQPlayerState>()->SetPlayerName(PlayerNickName);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue,
				FString::Printf(TEXT("PlayerNickName: %s"), *GetPlayerState<AQPlayerState>()->GetPlayerName()));
		}
	}
}

void AQStartPlayerController::SetSessionName(FString SessionName)
{
	NewSessionName = SessionName;
}

void AQStartPlayerController::CreateSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}
	// NAME_GameSession : 전역변수
	// 예: 플레이어 닉네임 기반 고유 세션 이름 설정
	NewSessionName = FString::Printf(TEXT("Session_%s"), *PlayerNickName);
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
	SessionSettings->NumPublicConnections = 1;                // 최대 플레이어 수
	SessionSettings->bAllowJoinInProgress = true;             // 진행 중 세션 참여 허용
	SessionSettings->bAllowJoinViaPresence = true;            // Presence를 통한 참여 허용
	SessionSettings->bShouldAdvertise = true;                 // 세션 공개 여부
	SessionSettings->bUsesPresence = true;                    // Presence 사용
	SessionSettings->bUseLobbiesIfAvailable = true;           // 로비 사용 가능

	// setting session name 
	SessionSettings->Set(FName(TEXT("SessionName")), NewSessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName(TEXT("Map")), FString(TEXT("Forest")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	FString HostName = PlayerNickName;

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

void AQStartPlayerController::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
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

void AQStartPlayerController::FindSession()
{
}

void AQStartPlayerController::OnFindSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void AQStartPlayerController::JoinSession()
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
	SessionSearch->QuerySettings.Set(FName(TEXT("SEARCH_PRESENCE")), true, EOnlineComparisonOp::Equals);
	//SessionSearch->QuerySettings.Set(FName(TEXT("SessionName")), NewSessionName, EOnlineComparisonOp::Equals);
	
	const ULocalPlayer* LocalPlayer = this->GetLocalPlayer();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void AQStartPlayerController::OnFindSessionComplete(bool bWasSuccessful)
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
	for (const FOnlineSessionSearchResult&  Result : SessionSearch->SearchResults)
	{
		FString OwningUser = Result.Session.OwningUserName;
		int32 MaxConnectionNum = Result.Session.SessionSettings.NumPublicConnections;
		int32 AvailableConnectionNum = Result.Session.NumOpenPublicConnections;
		FName SessionName = FName(*Result.GetSessionIdStr());

		/** @todo 조건에 따라 리스트에 보여줄 세션 거르기 */
		if (AvailableConnectionNum <= 0)
		{
			continue;
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue,
				FString::Printf(TEXT("Found Session - host : %s \n Session Name : %s \n Player num : %d/%d"),
					*OwningUser, *SessionName.ToString(), MaxConnectionNum-AvailableConnectionNum, MaxConnectionNum));
		}

		// Join 관련 delegate 등록
		if (!OnlineSessionInterface.IsValid()) return;
		OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

		// JoinSession
		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, Result);
		break;
	}
}

void AQStartPlayerController::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid()) return;

	// Returns the platform specific connection information for joining the match
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinSession: Resolved Address = %s"), *Address);
		this->ClientTravel(Address, TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("JoinSession: Failed to resolve connect string for session: %s"), *SessionName.ToString());
	}
}
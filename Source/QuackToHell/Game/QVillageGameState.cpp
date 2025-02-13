// Copyright_Team_AriAri


#include "Game/QVillageGameState.h"

#include <string>

#include "QGameModeVillage.h"
#include "QLogCategories.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/QPlayerState.h"
#include "UI/QVillageTimerWidget.h"
#include "UI/QVillageUIManager.h"
#include "UObject/ConstructorHelpers.h"


AQVillageGameState::AQVillageGameState()
{
	bReplicates = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("/Game/Blueprints/UI/WBP_ConversationTest"));
	if (WidgetFinder.Succeeded())
	{
		StartLevelWidget = WidgetFinder.Class;
	}

	// 초기화
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	NPCList = {};
	ConversationList = FConversationList(); // 명시적 초기화
	EvidenceList = FEvidenceList();
}

void AQVillageGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority()) // 서버에서만 실행
	{
		ServerLeftTimeUntilTrial += DeltaSeconds;
		ForceNetUpdate();

		MulticastRPCUpdateServerTime();
		if (GetNetMode() == NM_ListenServer) // ✅ Listen 서버의 "호스트 클라이언트"에서는 직접 실행
		{
			MulticastRPCUpdateServerTime_Implementation();
		}
	}
}

void AQVillageGameState::MulticastRPCUpdateServerTime_Implementation()
{
	TObjectPtr<UQVillageTimerWidget> VillageTimerUI = Cast<UQVillageTimerWidget>(AQVillageUIManager::GetInstance(GetWorld())->GetActivedVillageWidgets()[EVillageUIType::VillageTimer]);
	if (VillageTimerUI)
	{
		VillageTimerUI->UpdateServerTimeToUITime(ServerLeftTimeUntilTrial, TimeUntilTrialMax);
	}
	else
	{
		UE_LOG(LogLogic, Log, TEXT("Get VillageTimerUI failed"));
	}
}

void AQVillageGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AQVillageGameState, ServerLeftTimeUntilTrial);
	
	DOREPLIFETIME(AQVillageGameState, NPCList);
	DOREPLIFETIME(AQVillageGameState, ConversationList);
	DOREPLIFETIME(AQVillageGameState, EvidenceList);
}

// 대화와 관련한 함수들 -------------------------------------------------------------------------------------- //
const int32 AQVillageGameState::AddConversationRecord(EConversationType ConversationType, int32 ListenerID, int32 SpeakerID, FDateTime Timestamp, const FString& Message)
{
	// 클라이언트에서 이 함수에 접근하려고 하면 -1 return
	if (!HasAuthority()) return -1;
	if (!this)
	{
		UE_LOG(LogTemp, Error, TEXT("AQVillageGameState is invalid"));
		return -1;
	}
	
	AQGameModeVillage* GameModeVillage = Cast<AQGameModeVillage>(GetWorld()->GetAuthGameMode());
	if (GameModeVillage)
	{
		int32 ConversationID = GameModeVillage->ConversationIDCount++;
		FConversationRecord NewConversationRecord(ConversationID, ConversationType, SpeakerID, ListenerID, Timestamp, Message);
		ConversationList.AddConversation(NewConversationRecord);

		return ConversationID;
	}
	else
	{
		return -1;
	}
}

const int32 AQVillageGameState::AddEvidence(FString EvidenceName, FString EvidenceDescription, FString EvidenceImagePath)
{
	// 클라이언트에서 이 함수에 접근하려고 하면 -1 return
	if (!HasAuthority()) return -1;

	// @todo 게임모드 관련 
	// AQGameModeVillage* GameModeVillage = Cast<AQGameModeVillage>(GetWorld()->GetAuthGameMode());
	// int32 EvidenceID = GameModeVillage->EvidenceIDCount++;
	int32 EvidenceID = 1234;
	FEvidence NewEvidence(EvidenceID, EvidenceName, EvidenceDescription, EvidenceImagePath);
	EvidenceList.AddEvidence(NewEvidence);

	return EvidenceID;
}

void AQVillageGameState::RemoveEvidence(int32 EvidenceID)
{
	// 클라이언트에서 이 함수에 접근하려고 하면 바로 return
	if (!HasAuthority()) return;
	
	EvidenceList.RemoveEvidence(EvidenceID);
}

void AQVillageGameState::RemoveAllEvidence()
{
	// 클라이언트에서 이 함수에 접근하려고 하면 바로 return
	if (!HasAuthority()) return;
	
	EvidenceList.RemoveAllEvidence();
}

const FConversationRecord* AQVillageGameState::GetRecordWithConvID(int32 ConversationID) const
{
	return ConversationList.GetRecordWithConvID(ConversationID);
}

const TArray<FConversationRecord> AQVillageGameState::GetRecordWithPlayerID(int32 PlayerID) const{
	return ConversationList.GetRecordWithID(PlayerID);
}

const TArray<FConversationRecord> AQVillageGameState::GetRecordWithNPCID(int32 NPCID) const
{
	return ConversationList.GetRecordWithID(NPCID);
}

const FEvidence* AQVillageGameState::GetEvidenceWithID(int32 EvidenceID) const
{
	return EvidenceList.GetEvidenceWithID(EvidenceID);
}

const FEvidence* AQVillageGameState::GetEvidenceWithName(FString EvidenceName) const
{
	return EvidenceList.GetEvidenceWithName(EvidenceName);
}

const TArray<FEvidence> AQVillageGameState::GetEvidencesWithPlayerID() const
{
	int32 PlayerID = GetWorld()->GetFirstPlayerController()->GetPlayerState<AQPlayerState>()->GetPlayerId();
	return EvidenceList.GetEvidencesWithPlayerID(PlayerID);
}

void AQVillageGameState::BeginPlay()
{
	Super::BeginPlay();
	// 이부분을 주석해제하면 테스트 결과 확인가능//
	/*
	UUserWidget* StartWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), StartLevelWidget);
	if (StartWidget)
	{
		// 위젯을 화면에 추가
		StartWidget->AddToViewport();
	}
	*/
}

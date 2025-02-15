// Copyright_Team_AriAri


#include "Player/QPlayerState.h"
#include "QLogCategories.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Game/QVillageGameState.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

AQPlayerState::AQPlayerState()
{
	/*멀티플레이 관련*/
	bReplicates = true;
	PlayerConversationState = EConversationType::None;
}

void AQPlayerState::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AQVillageGameState>(GetWorld()->GetGameState());
	if (GameState == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AQPlayerState::BeginPlay Can't find GameState."));
	}
}

void AQPlayerState::AddStateTag(FGameplayTag NewStateTag)
{
	PlayerStateTags.AddTag(NewStateTag);
	UE_LOG(LogLogic, Log, TEXT("플레이어 상태 태그 추가: %s"), *NewStateTag.ToString());
}

void AQPlayerState::RemoveStateTag(FGameplayTag StateTag)
{
	PlayerStateTags.RemoveTag(StateTag);
	UE_LOG(LogLogic, Log, TEXT("플레이어 상태 태그 제거: %s"), *StateTag.ToString());
}

bool AQPlayerState::HasStateTag(FGameplayTag StateTag) const
{
	return PlayerStateTags.HasTagExact(StateTag);
}

void AQPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQPlayerState, EvidenceIDInHand);
	DOREPLIFETIME(AQPlayerState, PlayerConversationState);
}

const FConversationRecord* AQPlayerState::GetRecordWithConvID(int32 ConversationID) const
{
	// GameState에 저장되어 있는 대화기록 리스트에서 ID가 일치하는 대화기록 찾기
	const FConversationRecord* Record = GameState->GetRecordWithConvID(ConversationID);
	if (Record == nullptr) return nullptr;

	/** @todo  지금은 PlayerID 할당이 제대로 되어있지 않으므로 주석처리. 구현한 이후 다시 주석해제 필요 */
	// 로컬 플레이어가 해당 대화기록에 참여한 사람일 경우에만 대화기록 반환하기
	// if (Record->GetSpeakerID() != GetPlayerId() && Record->GetListenerID() != GetPlayerId()) return nullptr;
	
	return Record;
}

const TArray<FConversationRecord> AQPlayerState::GetRecordWithPlayerID() const
{
	//AQVillageGameState* VillageGameState = GetWorld() ? GetWorld()->GetGameState<AQVillageGameState>() : nullptr;

	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("AQPlayerState GetRecordWithPlayerID: VillageGameState is nullptr!"));
		return TArray<FConversationRecord>();
	}
	if (GameState->GetConversationList().GetConversationList().Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AQPlayerState GetRecordWithPlayerID: ConversationList is empty!"));
		return TArray<FConversationRecord>();
	}
	
	return GameState->GetRecordWithPlayerID(GetPlayerId());
}

const TArray<FConversationRecord> AQPlayerState::GetRecrodWithNPCID(int32 NPCID) const
{
	return GameState->GetRecordWithNPCID(NPCID);
}

const FEvidence* AQPlayerState::GetEvidenceWithID(int32 EvidenceID) const
{
	return GameState->GetEvidenceWithID(EvidenceID);
}

const FEvidence* AQPlayerState::GetEvidenceWithName(FString EvidenceName) const
{
	return GameState->GetEvidenceWithName(EvidenceName);
}

const TArray<FEvidence> AQPlayerState::GetEvidencesWithPlayerID() const
{
	return GameState->GetEvidencesWithPlayerID();
}

// -------------------------------------------------------------------------- //
/*

void AQPlayerState::PrintEvidence(int32 EvidenceID, FString EvidenceName) const
{
	const FEvidence* EvWithName = GetEvidenceWithName(EvidenceName);
	const FEvidence* EvWithID = GetEvidenceWithID(EvidenceID);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
				FString::Printf(TEXT("Found Evidence with ID - Evidence Name: %s, EvidenceID: %d"), *EvWithID->GetName(), EvWithID->GetID()));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
					FString::Printf(TEXT("Found Evidence with Name - Evidence Name: %s, EvidenceID: %d"), *EvWithName->GetName(), EvWithName->GetID()));
	}
}

void AQPlayerState::PrintAllEvidence() const
{
	const TArray<FEvidence>& PlayerEvidences = GetEvidencesWithPlayerID();
	if (GEngine)
	{
		for (const auto& Evidence : PlayerEvidences)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
				FString::Printf(TEXT("Found All Evidence - Evidence Name: %s, EvidenceID: %d"), *Evidence.GetName(), Evidence.GetID()));
		}
	}
}

void AQPlayerState::PrintConversation(int32 ConversationID) const
{
	AQVillageGameState* VillageGameState = GetWorld() ? GetWorld()->GetGameState<AQVillageGameState>() : nullptr;

	if (!VillageGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("PrintConversation: VillageGameState is nullptr!"));
		return;
	}
	if (GameState->GetConversationList().GetConversationList().Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetRecordWithConvID: ConversationList is empty!"));
		return;
	}
	else
	{
		for (const auto& Record : GameState->GetConversationList().GetConversationList())
		{
			UE_LOG(LogTemp, Log, TEXT("GetRecordWithConvID: ConversationList Record ID - %d"), Record.GetConversationID())
		}
	}
	const FConversationRecord* Record = GetRecordWithConvID(ConversationID);
	if (Record == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PrintConversation: Record is nullptr!"));
		return;
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
				FString::Printf(TEXT("Found Conversation with ID - Speaker: %d, Listener: %d at %s \n Conversation message: %s"), Record->GetSpeakerID(), Record->GetListenerID(), *Record->GetTimestamp().ToString()));
	}
}

void AQPlayerState::PrintAllConversation(int32 SpeakerID) const
{
	const TArray<FConversationRecord> Records = GetRecordWithPlayerID();
	if (GEngine)
	{
		for (const auto& Record : Records)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
			FString::Printf(TEXT("Found Conversation with Speaker - Speaker: %d, Listener: %d at %s\n Conversation message: %s"), Record.GetSpeakerID(), Record.GetListenerID(), *Record.GetTimestamp().ToString(),*Record.GetMessage()));
		}
	}
}

void AQPlayerState::TestAddConversation()
{
	GameState = Cast<AQVillageGameState>(GetWorld()->GetGameState());
	FDateTime Timestamp = FDateTime::Now();
	int32 ConversationID = GameState->AddConversationRecord(EConversationType::P2N, 1000, 1001, Timestamp, FString::Printf(TEXT("TestAddConversation s: %d  l:%d"), 1000, 1001));
	PrintConversation(ConversationID);
	PrintAllConversation(1001);
}

void AQPlayerState::TestAddEvidence()
{
	GameState = Cast<AQVillageGameState>(GetWorld()->GetGameState());
	int32 EvidenceID1 = GameState->AddEvidence(TEXT("Evidence1"), TEXT("Evidence1 description"), TEXT("Evidence1 Image Path"));
	int32 EvidenceID2 = GameState->AddEvidence(TEXT("Evidence2"), TEXT("Evidence2 description"), TEXT("Evidence2 Image Path"));
	int32 EvidenceID3 = GameState->AddEvidence(TEXT("Evidence3"), TEXT("Evidence3 description"), TEXT("Evidence3 Image Path"));

	PrintEvidence(EvidenceID1, "Evidence1");
	PrintEvidence(EvidenceID2, "Evidence2");
	PrintAllEvidence();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, TEXT("Remove Evidence1"));
	}

	GameState->RemoveEvidence(EvidenceID1);
	PrintAllEvidence();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, TEXT("Remove all Evidence"));
	}
	
	GameState->RemoveAllEvidence();
	PrintAllEvidence();
}
*/
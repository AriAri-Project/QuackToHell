// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Character/QNPC.h"
#include "GameData/QConversationData.h"
#include "GameData/QEvidenceData.h"
#include "GameFramework/GameState.h"
#include "QVillageGameState.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQVillageGameState : public AGameState
{
	GENERATED_BODY()

public:
	AQVillageGameState();

	virtual void Tick(float DeltaSeconds) override;
public:
	/**
	 * @brief 타이머가 울린 뒤, 마을활동을 마무리하는 함수입니다.
	 * 
	 */
	UFUNCTION(NetMulticast, Reliable)
	void EndVillageActivity();

private:
	float TimeUntilTrialMax = 60 * 7;
	
	/** @brief 재판까지 남은 시간 */
	UPROPERTY(Replicated)
	float ServerLeftTimeUntilTrial = 0.0f;
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCUpdateServerTime();

private:
	UPROPERTY(EditAnywhere, Replicated)
	TArray<TObjectPtr<AQNPC>> NPCList;

	UPROPERTY(EditAnywhere, Replicated)
	FEvidenceList EvidenceList;
	
	UPROPERTY(EditAnywhere, Replicated)
	FConversationList ConversationList;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 서버에서 실행되는 함수
	const int32 AddConversationRecord(EConversationType ConversationType, int32 ListenerID, int32 SpeakerID, FDateTime Timestamp, const FString& Message);
	const int32 AddEvidence(FString EvidenceName, FString EvidenceDescription, FString EvidenceImagePath);
	void RemoveEvidence(int32 EvidenceID);
	void RemoveAllEvidence();

	// 클라 접근용 Get Set 함수
	const FEvidenceList& GetEvidenceList() const
	{
		return EvidenceList;
	}
	const FConversationList& GetConversationList() const
	{
		return ConversationList;
	}
	const FConversationRecord* GetRecordWithConvID(int32 ConversationID) const;
	const TArray<FConversationRecord> GetRecordWithPlayerID(int32 PlayerID) const;
	const TArray<FConversationRecord> GetRecordWithNPCID(int32 NPCID) const;
	const FEvidence* GetEvidenceWithID(int32 EvidenceID) const;
	const FEvidence* GetEvidenceWithName(FString EvidenceName) const;
	const TArray<FEvidence> GetEvidencesWithPlayerID() const;

	virtual void BeginPlay() override;
	
	TSubclassOf<UUserWidget> StartLevelWidget;
};

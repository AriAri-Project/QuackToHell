// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "GameplayTagContainer.h" // GameplayTagContainer 사용
#include "Game/QVillageGameState.h"
#include "GameData/QConversationData.h"
#include "GameData/QEvidenceData.h"
#include "QPlayerState.generated.h"


/**
 * @author 전유진
 * @brief player state클래스입니다.
 */

UCLASS()
class QUACKTOHELL_API AQPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AQPlayerState();

	virtual void BeginPlay() override;
	/**
	 * @brief 플레이어의 상태를 관리하는 GameplayTags.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameplayTags")
	FGameplayTagContainer PlayerStateTags;
	/**
	 * @brief 새로운 태그를 추가합니다.
	 * @param NewStateTag
	 */
	void AddStateTag(FGameplayTag NewStateTag);
	/**
	 * @brief 태그를 지웁니다.
	 * @param 지울 태그
	 */
	void RemoveStateTag(FGameplayTag StateTag);
	/**
	 * @brief 태그를 갖고있는지 체크합니다.
	 * @param 비교할 태그
	 */
	bool HasStateTag(FGameplayTag StateTag) const;

private:
	UPROPERTY(VisibleAnywhere,Category = "GameplayTags")
	AQVillageGameState* GameState;
	
	/** @brief 플레이어의 대화 상태 */
	UPROPERTY(Replicated)
	EConversationType PlayerConversationState = EConversationType::None;
	
	/**  @breif 플레이어가 소지한 증거 정보의 ID가 저장되어 있는 Array */
	UPROPERTY(Replicated)
	TArray<int32> EvidenceIDInHand;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


public:
	EConversationType GetPlayerConversationState() const
	{
		return PlayerConversationState;
	}

	void SetPlayerConverstationState(EConversationType NewState)
	{
		UE_LOG(LogTemp, Log, TEXT("SetPlayerConverstationState: 기존 상태 = %s, 새로운 상태 = %s"), 
				   *UEnum::GetValueAsString(PlayerConversationState), *UEnum::GetValueAsString(NewState));

		PlayerConversationState = NewState;
	}
	
	TArray<int32> GetEvidenceIDInHand() const
	{
		return EvidenceIDInHand;
	}

	/** @brief 대화기록ID를 이용해 대화기록 조회. 처음 대화기록을 저장할 때 저장한 대화기록에 해당하는 ID를 반환해주므로 그것을 이용하면 됨.*/
	const FConversationRecord* GetRecordWithConvID(int32 ConversationID) const;
	/** @brief PlayerID를 이욯해 대화기록 조회. 로컬 Player가 Speaker와 Listener로 참여한 모든 대화기록을 배열 형태로 반환*/
	const TArray<FConversationRecord> GetRecordWithPlayerID() const;
	/** @brief 특정 NPCID를 이용해 대화기록 조회.*/
	const TArray<FConversationRecord> GetRecrodWithNPCID(int32 NPCID) const;
	/** @brief 증거ID를 이용해 증거 조회. */
	const FEvidence* GetEvidenceWithID(int32 EvidenceID) const;
	/** @brief 증거 이름을 이용해 증거 조회*/
	const FEvidence* GetEvidenceWithName(FString EvidenceName) const;
	/** @brief 플레이어가 소유한 증거를 배열 형태로 반환 */
	const TArray<FEvidence> GetEvidencesWithPlayerID() const;

	/*
	// for testing conversation & evidence system -----------------------
	void PrintEvidence(int32 EvidenceID, FString EvidenceName) const;
	void PrintAllEvidence() const;
	void PrintConversation(int32 ConversationID) const;
	void PrintAllConversation(int32 SpeakerID) const;

	UFUNCTION(BlueprintCallable)
	void TestAddConversation();
	UFUNCTION(BlueprintCallable)
	void TestAddEvidence();
	*/
};
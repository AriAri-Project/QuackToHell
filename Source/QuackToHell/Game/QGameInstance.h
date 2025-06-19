// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameData/QConversationData.h"
#include "GameData/QEvidenceData.h"
#include "QGameInstance.generated.h"

/**
* @author 전유진.
* @brief 증거물 생성완료시점 감지 이벤트
*/
DECLARE_MULTICAST_DELEGATE(FOnEvidenceDataGenerated);
USTRUCT(BlueprintType)
struct FStatementEntry
{
	GENERATED_BODY()
public:
	// 누가 보낸 발언인지
	UPROPERTY()
	bool bServer;

	// 실제 발언 텍스트
	UPROPERTY()
	FString Statement;
};

class AQNPC;
/**
 * 
 */
UCLASS()
class QUACKTOHELL_API UQGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	//헬퍼메서드
	bool IsServer() const
	{
		if (UWorld* World = GetWorld())
		{
			ENetMode Mode = World->GetNetMode();
			return (Mode == NM_DedicatedServer || Mode == NM_ListenServer);
		}
		return false;
	}

	bool IsClient() const
	{
		if (UWorld* World = GetWorld())
		{
			return (World->GetNetMode() == NM_Client);
		}
		return false;
	}
public:
	UFUNCTION(Server, Reliable)
	void ServerRPCSaveServerAndClientStatement(const FString& InputText, bool bServer);
public:
	void SetOpeningStetementText(FString NewText);

	/*             */
	/**
	 * @author 전유진.
	 */
public:
	// 레벨 언로드 전에 보관할 NPC ID 리스트
	UPROPERTY()
	TArray<int32> SavedNPCIDs = {
		2000, // Defendant
		2001, // Jury #1
		2002, // Jury #2
		2003, // Jury #3
	};

	// NPC ID 추가

	void AddNPCID(int32 NPCID)
	{
		if (!SavedNPCIDs.Contains(NPCID))
		{
			SavedNPCIDs.Add(NPCID);
		}
	}

	// 저장된 전체 NPC ID 반환
	const TArray<int32>& GetSavedNPCIDs() const
	{
		return SavedNPCIDs;
	}
private:
	/** 플레이어가 습득한 증거 ID 리스트 */
	UPROPERTY()
	TArray<FEvidence> PlayerInventoryEvidences;

public:
	/** @brief 플레이어가 증거를 인벤토리에 추가합니다 */
	UFUNCTION(BlueprintCallable, Category = "Evidence")
	void AddInventoryEvidence(FEvidence NewEvidence);

	/** @brief 플레이어 인벤토리 내 증거 ID 목록을 반환합니다 */
	UFUNCTION(BlueprintCallable, Category = "Evidence")
	const TArray<FEvidence>& GetInventoryEvidences() const;
	/*             */
public:
	/**
	 * @author 전유진.
	 * @brief 증거물 생성완료시점 감지 이벤트
	 */
	FOnEvidenceDataGenerated OnEvidenceJsonGenerated;
public:
	UQGameInstance();

	// 프롬프트 삭제 및 3초 후 재생성 실행
	void SchedulePromptRegeneration();
	void StartPromptGeneration();

protected:
	/** GameInstance 초기화 시점에 호출됩니다 */
	virtual void Init() override;




private:
	// ID 관리
	static int32 PlayerIDCount;				const int32 PlayerIDInit = 1000;
	static int32 NPCIDCount;				const int32 NPCIDInit = 2000;
	static int32 ConversationIDCount;		const int32 ConversationIDInit = 3000;
	static int32 EvidenceIDCount;			const int32 EvidenceIDInit = 4000;

	UPROPERTY(EditAnywhere, Replicated)
	TArray<TObjectPtr<AQNPC>> NPCList;

	UPROPERTY(EditAnywhere, Replicated)
	FEvidenceList EvidenceList;
	
	UPROPERTY(EditAnywhere, Replicated)
	FConversationList ConversationList;

	// NetMode 체크 함수
	ENetMode GetLocalNetMode();

	FTimerHandle TimerHandle_PromptRegeneration;
	
	// 프로퍼티 리플레이케이션을 위한 변수 관리
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 서버에서 실행되는 함수
	const int32 AddConversationRecord(EConversationType ConversationType, int32 ListenerID, int32 SpeakerID, FDateTime Timestamp, const FString& Message);
	const int32 AddEvidence(FString EvidenceName, FString EvidenceDescription, FString EvidenceImagePath);
	void RemoveEvidence(int32 EvidenceID);
	void RemoveAllEvidence();
	
	// 클라 접근 함수
	/* 대화기록 증거 저장 시스템 */
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
	const TArray<FEvidence> GetEvidencesWithPlayerID(int32 PlayerID) const;
};

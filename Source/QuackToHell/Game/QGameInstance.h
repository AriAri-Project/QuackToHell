// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameData/QConversationData.h"
#include "GameData/QEvidenceData.h"
#include "QGameInstance.generated.h"

class AQNPC;
/**
 * 
 */
UCLASS()
class QUACKTOHELL_API UQGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UQGameInstance();

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

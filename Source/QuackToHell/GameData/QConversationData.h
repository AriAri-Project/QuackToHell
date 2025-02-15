// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QConversationData.generated.h"

/**
 * @author 유서현
 * @brief 대화 기록 데이터 저장을 위한 자료구조 및 열거형이 정의되어있는 헤더파일입니다.
 */

/**
 * @brief 대화 유형을 나타내는 Enum
 */
UENUM(BlueprintType)
enum class EConversationType : uint8
{
	None UMETA(DisplayName = "None"),
	PStart UMETA(DisplayName = "Start P"),
	P2N UMETA(DisplayName = "P2N"),  // 플레이어 ↔ NPC 대화
	N2N UMETA(DisplayName = "N2N"),    // NPC ↔ NPC 대화
	N2NStart UMETA(DisplayName = "N2NStart "),
	NMonologue UMETA(DisplayName = "NMonologue") // NPC 혼잣말
};

USTRUCT(BlueprintType)
struct FConversationRecord
{
	GENERATED_BODY()
private:
	UPROPERTY()
	int32 ConversationID;

	UPROPERTY()
	EConversationType ConversationType;
	
	UPROPERTY()
	int32 ListenerID;

	UPROPERTY()
	int32 SpeakerID;

	UPROPERTY()
	FDateTime Timestamp;

	UPROPERTY()
	FString Message;

public:
	FConversationRecord(){}
	
	FConversationRecord(int32 ConversationID, EConversationType ConversationType, int32 SpeakerID, int32 ListenerID, FDateTime Timestamp, const FString& Message)
		: ConversationID(ConversationID), ConversationType(ConversationType), ListenerID(ListenerID), SpeakerID(SpeakerID), Timestamp(Timestamp), Message(Message){}

	bool operator==(const FConversationRecord& Other) const
	{
		return ConversationID == Other.ConversationID;
	}

	FConversationRecord& operator=(const FConversationRecord& Other)
	{
		if (this != &Other)
		{
			ConversationID = Other.ConversationID;
			ListenerID = Other.ListenerID;
			SpeakerID = Other.SpeakerID;
			Timestamp = Other.Timestamp;
			Message = Other.Message;
		}
		return *this;
	}
	
	int32 GetConversationID() const {return ConversationID;}
	EConversationType GetConversationType() const {return ConversationType;}
	int32 GetListenerID() const {return ListenerID;}
	int32 GetSpeakerID() const {return SpeakerID;}
	FDateTime GetTimestamp() const {return Timestamp;}
	FString GetMessage() const {return Message;}
};

USTRUCT(BlueprintType)
struct FConversationIDWrapper
{
	GENERATED_BODY()
public:
	TArray<int32> IDs;
};

USTRUCT()
struct FConversationList
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FConversationRecord> ConversationList;

	FConversationList()
	{
		ConversationList = TArray<FConversationRecord>();  // 강제 초기화
	}

	void AddConversation(const FConversationRecord& ConversationRecord)
	{
		if (ConversationList.Contains(ConversationRecord)) return;

		ConversationList.Add(ConversationRecord);
	}
	
	const TArray<FConversationRecord>& GetConversationList() const
	{
		return ConversationList;
	}

	// ConversationID에 해당하는 ConversationRecord를 반환
	const FConversationRecord* GetRecordWithConvID(int32 ConversationID) const
	{
		for (const auto& ConversationRecord : ConversationList)
		{
			if (ConversationRecord.GetConversationID() == ConversationID)
			{
				return &ConversationRecord;
			}
		}
		return nullptr;
	}

	// Player가 소유하고 있는 대화기록에 대한 정보만 반환
	const TArray<FConversationRecord> GetRecordWithID(int32 ID) const
	{
		if (this == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("FConversationList instance is nullptr!"));
			return TArray<FConversationRecord>();
		}

		if (!ensureAlwaysMsgf(ConversationList.GetAllocatedSize() > 0, TEXT("ConversationList is not properly allocated!")))
		{
			UE_LOG(LogTemp, Error, TEXT("ConversationList is not initialized!"));
			return TArray<FConversationRecord>();
		}
		
		// 대화기록이 없다면 바로 return
		if (ConversationList.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("ConversationList is empty in GetRecordWithID!"));
			return TArray<FConversationRecord>();
		}
		
		return ConversationList.FilterByPredicate([ID](const FConversationRecord& Record)
		{
			// Player와 관련된 대화가 아니라면 제외
			if (Record.GetConversationType() != EConversationType::P2N && Record.GetConversationType() != EConversationType::PStart)
			{
				return false;
			}
			return (Record.GetSpeakerID() == ID || Record.GetListenerID() == ID);
		});
	}
};

UCLASS()
class QUACKTOHELL_API UQConversationData : public UObject
{
	GENERATED_BODY()
};
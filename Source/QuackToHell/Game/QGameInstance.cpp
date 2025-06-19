// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/QGameInstance.h"
#include "GameFramework/Actor.h"
#include "QLogCategories.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "GodFunction.h"
#include "TimerManager.h"
#include "QGameStateCourt.h"
#include <UObject/FastReferenceCollector.h>

int UQGameInstance::PlayerIDCount;
int UQGameInstance::NPCIDCount;
int UQGameInstance::ConversationIDCount;
int UQGameInstance::EvidenceIDCount;

void UQGameInstance::ServerRPCSaveServerAndClientStatement_Implementation(const FString& InputText, bool bServer)
{
	AQGameStateCourt* GS = GetWorld()->GetGameState<AQGameStateCourt>();

	if (bServer) {
		GS->GetOpeningStatements()[0].bServer = true;
		GS->GetOpeningStatements()[0].Statement= InputText;
	}
	else {
		GS->GetOpeningStatements()[1].bServer = false;
		GS->GetOpeningStatements()[1].Statement = InputText;
	}
}

void UQGameInstance::Init()
{
	Super::Init();
	// 맵 로드 직후 호출될 델리게이트 바인딩
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UQGameInstance::OnPostLoadMap);
}

void UQGameInstance::RegisterPersistentActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (!PersistentActors.Contains(Actor))
	{
		PersistentActors.Add(Actor);
	}
}

void UQGameInstance::OnPostLoadMap(UWorld* LoadedWorld)
{
	if (!LoadedWorld)
	{
		return;
	}

	ULevel* PersistentLevel = LoadedWorld->PersistentLevel;
	// 뒤에서부터 순회하며 제거/이동
	for (int32 Index = PersistentActors.Num() - 1; Index >= 0; --Index)
	{
		TWeakObjectPtr<AActor> WeakPtr = PersistentActors[Index];
		AActor* Actor = WeakPtr.Get();

		if (!Actor || !IsValid(Actor) || Actor->IsActorBeingDestroyed())
		{
			// 이미 파괴된 액터는 리스트에서 제거
			PersistentActors.RemoveAt(Index);
			continue;
		}

		// Persistent Level이 아닌 레벨에 있으면 이동
		ULevel* CurrentLevel = Actor->GetLevel();
		if (CurrentLevel && CurrentLevel != PersistentLevel)
		{
			// 서브레벨의 액터 배열에서 제거
			CurrentLevel->Actors.Remove(Actor);
			// PersistentLevel의 배열에 추가
			PersistentLevel->Actors.Add(Actor);
			// 액터의 Outer를 PersistentLevel로 변경 (Rename)
			Actor->Rename(nullptr, PersistentLevel);
		}
	}
}

TArray<TWeakObjectPtr<AActor>> UQGameInstance::GetPersistentActors() const
{
	return PersistentActors;
}


void UQGameInstance::SetOpeningStetementText(FString NewText)
{
	//서버에저장
	ServerRPCSaveServerAndClientStatement(NewText, IsServer());
	
}

void UQGameInstance::AddInventoryEvidence(FEvidence NewEvidence)
{
	// 새 증거의 ID 가져오기
	const int32 NewID = NewEvidence.GetID();

	// PlayerInventoryEvidences 에 이미 같은 ID가 있는지 확인
	for (const FEvidence& Evidence : PlayerInventoryEvidences)
	{
		if (Evidence.GetID() == NewID)
		{
			// 이미 인벤토리에 존재하면 바로 리턴
			return;
		}
	}

	// 중복 없으면 추가
	PlayerInventoryEvidences.Add(NewEvidence);
}

const TArray<FEvidence>& UQGameInstance::GetInventoryEvidences() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return PlayerInventoryEvidences;
}



UQGameInstance::UQGameInstance()
{
	// 데이터 초기화
	PlayerIDCount = PlayerIDInit;
	NPCIDCount = NPCIDInit;
	ConversationIDCount = ConversationIDInit;
	EvidenceIDCount = EvidenceIDInit;



	NPCList = {};
	ConversationList = FConversationList(); // 명시적 초기화
	EvidenceList = FEvidenceList();
}

ENetMode UQGameInstance::GetLocalNetMode()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogLogic, Log, TEXT("UQGameInstance::GetLocalNetMode - World is nullptr"));
		return NM_MAX;
	}
	return World->GetNetMode();
}

void UQGameInstance::SchedulePromptRegeneration()
{
	//UE_LOG(LogTemp, Log, TEXT("🔄 프롬프트 삭제 후 3초 후 재생성 예약"));

	//// 기존 프롬프트 삭제
	//UGodFunction::DeleteOldPromptFiles();

	//// ✅ 즉시 프롬프트 생성 시작
	//StartPromptGeneration();
}

void UQGameInstance::StartPromptGeneration()
{
	/*UE_LOG(LogTemp, Log, TEXT("🕒 PromptToDefendant.json 생성 시작"));

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ GetWorld()가 NULL이라 프롬프트 생성 실패!"));
		return;
	}

	UGodFunction::GenerateDefendantPrompt(World, [this]()
		{
			UE_LOG(LogTemp, Log, TEXT("✅ PromptToDefendant.json 생성 완료! NPC 프롬프트 생성 시작"));
			UGodFunction::GenerateJuryNPC(GetWorld(), 1);
		});*/
}

void UQGameInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UQGameInstance, NPCList);

	DOREPLIFETIME(UQGameInstance, ConversationList);
	DOREPLIFETIME(UQGameInstance, EvidenceList);
}

const int32 UQGameInstance::AddConversationRecord
(EConversationType ConversationType, int32 ListenerID, int32 SpeakerID, FDateTime Timestamp, const FString& Message)
{
	// 클라이언트에서 이 함수에 접근하려고 하면 -1 return
	if (GetLocalNetMode() == NM_Client) return -1;
	if (!this)
	{
		UE_LOG(LogTemp, Error, TEXT("UQGameInstance is invalid"));
		return -1;
	}

	// ID 부여 후 리스트에 새로운 대화기록 추가
	int32 ConversationID = ConversationIDCount++;
	FConversationRecord NewConversationRecord(ConversationID, ConversationType, SpeakerID, ListenerID, Timestamp, Message);
	ConversationList.AddConversation(NewConversationRecord);

	return ConversationID;
}

const int32 UQGameInstance::AddEvidence(FString EvidenceName, FString EvidenceDescription, FString EvidenceImagePath)
{
	// 클라이언트에서 이 함수에 접근하려고 하면 -1 return
	if (GetLocalNetMode() == NM_Client) return -1;
	if(!this)
	{
		UE_LOG(LogTemp, Error, TEXT("UQGameInstance is invalid"));
		return -1;
	}

	int32 EvidenceID = EvidenceIDCount++;
	FEvidence NewEvidence(EvidenceID, EvidenceName, EvidenceDescription, EvidenceImagePath);
	EvidenceList.AddEvidence(NewEvidence);

	return EvidenceID;
}

void UQGameInstance::RemoveEvidence(int32 EvidenceID)
{
	// 클라이언트에서 이 함수에 접근하려고 하면 -1 return
	if (GetLocalNetMode() == NM_Client) return;
	
	EvidenceList.RemoveEvidence(EvidenceID);
}

void UQGameInstance::RemoveAllEvidence()
{
	// 클라이언트에서 이 함수에 접근하려고 하면 -1 return
	if (GetLocalNetMode() == NM_Client) return;

	EvidenceList.RemoveAllEvidence();
}

const FConversationRecord* UQGameInstance::GetRecordWithConvID(int32 ConversationID) const
{
	const FConversationRecord* record = ConversationList.GetRecordWithConvID(ConversationID);
	return record;
}

const TArray<FConversationRecord> UQGameInstance::GetRecordWithPlayerID(int32 PlayerID) const
{
	return ConversationList.GetRecordWithID(PlayerID);
}

const TArray<FConversationRecord> UQGameInstance::GetRecordWithNPCID(int32 NPCID) const
{
	return ConversationList.GetRecordWithID(NPCID);
}

const FEvidence* UQGameInstance::GetEvidenceWithID(int32 EvidenceID) const
{
	return EvidenceList.GetEvidenceWithID(EvidenceID);
}

const FEvidence* UQGameInstance::GetEvidenceWithName(FString EvidenceName) const
{
	return EvidenceList.GetEvidenceWithName(EvidenceName);
}

const TArray<FEvidence> UQGameInstance::GetEvidencesWithPlayerID(int32 PlayerID) const
{
	return EvidenceList.GetEvidencesWithPlayerID(PlayerID);
}

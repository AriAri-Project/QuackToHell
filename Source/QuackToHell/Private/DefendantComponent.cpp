// Copyright_Team_AriAri

#include "DefendantComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "JsonObjectConverter.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Logging/LogMacros.h"

void UDefendantComponent::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("UDefendantComponent::BeginPlay() 실행됨 - NPC %s"), *NPCID);

    static int32 DefendantCounter = 2001;

    if (NPCID.IsEmpty())
    {
        NPCID = FString::FromInt(DefendantCounter);
        UE_LOG(LogTemp, Log, TEXT("NPCID 자동 할당됨: %s"), *NPCID);
    }

    // 피고인의 JSON 파일 경로 설정
    PromptFilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Prompt"), TEXT("PromptToDefendant.json"));

    UE_LOG(LogTemp, Log, TEXT("DefendantComponent - NPC %s는 %s를 사용합니다."), *NPCID, *PromptFilePath);

    // 피고인 데이터 로드
    if (!NPCID.IsEmpty())
        LoadPrompt();
}

void UDefendantComponent::PerformNPCLogic()
{
    UE_LOG(LogTemp, Log, TEXT("Defendant is preparing their case."));

    if (PromptContent.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Defendant's prompt file."));
        return;
    }

    // 피고인의 JSON 데이터를 로그로 출력 (디버깅용)
    UE_LOG(LogTemp, Log, TEXT("Defendant's prompt loaded: %s"), *PromptContent);
}


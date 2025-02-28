// Copyright_Team_AriAri


#include "JudgeComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UJudgeComponent::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("UJudgeComponent::BeginPlay() 실행됨 - 판사 NPC"));

    // 판사 프롬프트 파일 로드
    PromptFilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Prompt"), TEXT("PromptToJudge.json"));
    LoadPrompt();
}
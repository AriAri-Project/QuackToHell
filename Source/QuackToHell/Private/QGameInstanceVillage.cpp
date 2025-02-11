// Copyright_Team_AriAri


#include "QGameInstanceVillage.h"
#include "GodFunction.h"
#include "TimerManager.h"

void UQGameInstanceVillage::SchedulePromptRegeneration()
{
    UE_LOG(LogTemp, Log, TEXT("🔄 프롬프트 삭제 후 3초 후 재생성 예약"));

    // 기존 프롬프트 삭제
    UGodFunction::DeleteOldPromptFiles();

    // ✅ 즉시 프롬프트 생성 시작
    StartPromptGeneration();
}

void UQGameInstanceVillage::StartPromptGeneration()
{
    UE_LOG(LogTemp, Log, TEXT("🕒 PromptToDefendant.json 생성 시작"));

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
        });
}

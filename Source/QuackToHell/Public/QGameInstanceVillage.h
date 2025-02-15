// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "QGameInstanceVillage.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API UQGameInstanceVillage : public UGameInstance
{
    GENERATED_BODY()

public:
    // 프롬프트 삭제 및 3초 후 재생성 실행
    void SchedulePromptRegeneration();
    void StartPromptGeneration();

private:

    FTimerHandle TimerHandle_PromptRegeneration;
};

// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "NPCComponent.h"
#include "ResidentComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QUACKTOHELL_API UResidentComponent : public UNPCComponent
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    /**
     * @brief StartConversation()을 오버라이딩하여 NPC마다 개별 프롬프트 기반 대화 가능
     * @param PlayerInput 플레이어의 입력
     */
    virtual void StartConversation(FOpenAIRequest Request) override;

};

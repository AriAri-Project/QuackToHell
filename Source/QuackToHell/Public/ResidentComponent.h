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
     * @brief 주민이 플레이어의 질문에 대답하도록 NPCComponent에게 요청하는 함수
     * @param PlayerInput 플레이어의 입력 (질문)
     */
    UFUNCTION(BlueprintCallable, Category = "Resident")
    void AskResidentQuestion(const FString& PlayerInput);

    /**
     * @brief StartConversation()을 오버라이딩하여 NPC마다 개별 프롬프트 기반 대화 가능
     * @param PlayerInput 플레이어의 입력
     */
    virtual void StartConversation(const FOpenAIRequest& Request) override;
};

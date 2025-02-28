// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "NPCComponent.h"
#include "DefendantComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QUACKTOHELL_API UDefendantComponent : public UNPCComponent
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    /**
     * @brief 피고인의 데이터를 로드하고 논리를 수행하는 함수
     */
    virtual void PerformNPCLogic() override;

    /**
     * @brief 피고인의 모두진술을 AI에 요청하고, 응답을 서버로 전송하는 함수
     * @param Request FOpenAIRequest 구조체로, 대화의 주요 데이터를 포함
     *        - SpeakerID: 피고인의 ID
     *        - ListenerID: 플레이어의 ID
     *        - Prompt: 피고인이 기억하는 전생에 대한 서술
     * @return 응답은 `SendNPCResponseToServer()`를 통해 서버로 전송됨
     */
    UFUNCTION(BlueprintCallable, Category = "AI")
    void DefendantOpeningStatement(FOpenAIRequest Request);
};

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

};

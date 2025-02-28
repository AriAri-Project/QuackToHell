// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "NPCComponent.h"
#include "JudgeComponent.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API UJudgeComponent : public UNPCComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
};

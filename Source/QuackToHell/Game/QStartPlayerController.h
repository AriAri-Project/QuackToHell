// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "QStartPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQStartPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AQStartPlayerController();

private:
	virtual void BeginPlay() override;
};

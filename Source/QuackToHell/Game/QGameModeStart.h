// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h" // 필수 헤더
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "QGameModeStart.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQGameModeStart : public AGameModeBase
{
	GENERATED_BODY()
public:
	AQGameModeStart();
	virtual void BeginPlay() override;
};

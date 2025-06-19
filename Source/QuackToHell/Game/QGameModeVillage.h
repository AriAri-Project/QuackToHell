// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "QGameModeVillage.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQGameModeVillage : public AGameMode
{
	GENERATED_BODY()

public:
	AQGameModeVillage();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	
	/** @brief 재판장 Travel 함수 */
	UFUNCTION()
	void TravelToCourtMap();

private:
	// 3초 후 Prompt 생성 시작
	void StartPromptGeneration();

	// Timer Handle: 3초 대기 후 프롬프트 생성에 사용
	FTimerHandle TimerHandle_GeneratePrompt;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "QGameModeCourt.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQGameModeCourt : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void StartPlay() override;
public:
	AQGameModeCourt();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CourtOpeningDirection.generated.h"

/**
 * @author 전유진
 */
UCLASS()
class QUACKTOHELL_API UCourtOpeningDirection : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void AlertFinishDirection();
};

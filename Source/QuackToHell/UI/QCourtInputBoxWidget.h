// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QCourtInputBoxWidget.generated.h"

/**
 * @author 전유진
 * @brief 법원 내 입력 박스(플레이어가 메세지를 입력하는 곳)입니다. 
 */
UCLASS()
class QUACKTOHELL_API UQCourtInputBoxWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/**
	 * @brief 입력을 저장합니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSaveInput();
	
};

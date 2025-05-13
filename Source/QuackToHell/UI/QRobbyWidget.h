// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QRobbyWidget.generated.h"

/**
 * @author 전유진
 * @brief 로비 씬의 UI에 관련된 기능이 모아져있는 클래스입니다.
 */
UCLASS()
class QUACKTOHELL_API UQRobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/**
	 * @details 캡스톤용.
	 * @brief 서버에서 준비가 되었을 시, 시작버튼을 activate해 줍니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Button", Client, Reliable)
	void ActivateStartButton();

};

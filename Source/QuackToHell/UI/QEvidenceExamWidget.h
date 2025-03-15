// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QEvidenceExamWidget.generated.h"

/**
 * @author 전유진
 * @brief 증거조사 UI입니다. 
 * 최종적으로 선택 한 증거를 가지고 상대방과 의견논쟁을 하는 UI입니다. 
 */
UCLASS()
class QUACKTOHELL_API UQEvidenceExamWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	/**
	 * @brief 타이머 sizebox를 바인드
	 * @details sizebox안에는 타이머가 띄워진다.
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> TimerBox;
	/**
	 * @brief 인풋 sizebox를 바인드
	 * @details sizebox안에는 인풋창이 띄워진다.
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> InputBox;
protected:
	/**
	 * @brief 타이머 UI를 TimerBox sizebox에 달아줍니다.
	 * @details 블루프린트 노드에서 호출합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOnTimerUI();

	/**
	 * @brief inputbox UI를 InputBox sizebox에 달아줍니다.
	 * @details 블루프린트 노드에서 호출합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOnInputBoxUI();
};

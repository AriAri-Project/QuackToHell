// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QOpeningStatementWidget.generated.h"

/**
 * @author 전유진
 * @brief 재판장 모두진술 UI입니다.
 */
UCLASS()
class QUACKTOHELL_API UQOpeningStatementWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	/** 인벤토리에서 불러온 증거 이미지들로 그리드를 채웁니다 */
	UFUNCTION(BlueprintCallable)
	void PopulateEvidenceGrid();
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

private:
	/** 블루프린트에서 BindWidget 해 두세요 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> EvidenceGrid;

};

// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QDefaultVillageWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecordButtonPressed);

/**
 * @author 전유진
 * @brief 마을에 기본적으로 존재해야 하는 위젯입니다. (녹음기버튼, .. )
 */
UCLASS()
class QUACKTOHELL_API UQDefaultVillageWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void TurnOnGrandTitle();
	void TurnOnMiddleTitle();
	void SetGrandTitle(const FText& InText); 
	void SetMiddleTitle(const FText& InText); 
public:
	/**
	 * @ 녹음기 버튼이 눌리는 이벤트에 대한 델리게이트
	 */
	FOnRecordButtonPressed OnRecordButtonPressed;
public:
	/**
	 * @brief 버튼 상호작용을 차단합니다.
	 * 
	 */
	void BlockButtonsInteraction();
protected:
	/**
	 * @brief 타이머 sizebox를 바인드
	 * @details sizebox안에는 타이머가 띄워진다.
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> TimerBox;

	/**
	 * @brief 녹음기 버튼을 바인드.
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> WalkieTalkieButton;

	/**
	 * @brief 지도 버튼을 바인드.
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> MapButton;

	/**
	 * @brief 대제목을 바인드.
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GrandTitle;

	/**
	 * @brief 중제목을 바인드.
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> MiddleTitle;
	
protected:
	/**
	 * @brief 타이머 UI를 TimerBox sizebox에 달아줍니다.
	 * @details 블루프린트 노드에서 호출합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOnTimerUI();
	/**
	 * @brief 맵 버튼이 눌릴 시 맵 UI를 켭니다.
	 * 
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOnMapUI();
	/**
	 * @brief 녹음기 버튼이 눌릴 시 녹음기 UI를 켭니다.
	 *
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOnWalkieTakieUI();
};

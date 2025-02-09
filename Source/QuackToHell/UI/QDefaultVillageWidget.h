// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QDefaultVillageWidget.generated.h"

/**
 * @author 전유진
 * @brief 마을에 기본적으로 존재해야 하는 위젯입니다. (녹음기버튼, .. )
 */
UCLASS()
class QUACKTOHELL_API UQDefaultVillageWidget : public UUserWidget
{
	GENERATED_BODY()

private:
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

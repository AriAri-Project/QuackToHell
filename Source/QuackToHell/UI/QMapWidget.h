// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QMapWidget.generated.h"

/**
 * @author 전유진
 * @brief 지도위젯입니다.
 */
UCLASS()
class QUACKTOHELL_API UQMapWidget : public UUserWidget
{
	GENERATED_BODY()
protected:

	/**
	 * @brief 맵 버튼이 눌릴 시 맵 UI를 끕니다.
	 *
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOffMapUI();

	
};

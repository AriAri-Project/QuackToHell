// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QInventoryWidget.generated.h"

/**
 * @author 전유진
 * @brief 인벤토리 위젯입니다.
 */
UCLASS()
class QUACKTOHELL_API UQInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeDestruct() override;
	/**
	 * @brief. 유아이를 끕니다.
	 * 
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOffUI() const;
};

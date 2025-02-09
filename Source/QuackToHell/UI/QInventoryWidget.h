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
	/*바인드*/
	/**
	 * @brie 녹음위젯 컴포넌트 바인드
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UQRecordWidget> RecordWidget;

protected:
	
	/**
	 * @brief. 유아이를 끕니다.
	 * 
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOffUI() const;
	/**
	 * @brief. 녹음기록 위젯을 켭니다.
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOnRecordUI() const;
	/**
	 * @brief. 녹음기록 위젯을 끕니다.
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOffRecordUI() const;

};

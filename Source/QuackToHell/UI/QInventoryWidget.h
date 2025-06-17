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
	 * @brief 정보란 sizebox를 바인드
	 * @details sizebox안에는 녹음정보가 띄워지거나 증거물정보가 띄워진다. 
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> InformationBox;


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
	 * @brief. 증거물 위젯을 켭니다.
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOnEvidenceUI() const;
	/**
	 * @brief. 인포박스를 비웁니다.
	 */
	UFUNCTION(BlueprintCallable)
	void ClearInformationBox() const;

};

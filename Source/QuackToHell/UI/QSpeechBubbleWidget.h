// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QSpeechBubbleWidget.generated.h"

/** 
 * @author 전유진
 * @brief 말풍선 위젯입니다.   
 */
UCLASS()
class QUACKTOHELL_API UQSpeechBubbleWidget : public UUserWidget
{
	GENERATED_BODY()
//protected:
//	
//	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
//private:
//	/**
//	 * @brief 항상 카메라 기준으로 말풍선이 자연스럽게 보이도록합니다.
//	 */
//	void UpdateSpeechBubbleRotation();

public:
	/**
	 * @brief 말풍선 UI 컴포넌트를 킵니다.
	 *
	 */
	void TurnOnSpeechBubble();
	/**
	 * @brief 말풍선 UI 컴포넌트를 끕니다.
	 *
	 */
	void TurnOffSpeechBubble();
	/**
	 * @brief 말풍선의 글씨를 업데이트합니다.
	 * @param 새로 띄울 텍스트를 인자로 넣습니다.
	 */
	void UpdateText(const FString& Text);
private:
	/**
	 * @brie Text 컴포넌트 바인드
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextBlock;
};

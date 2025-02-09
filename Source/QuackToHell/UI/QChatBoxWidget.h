// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QChatBoxWidget.generated.h"

/**
 * @author 전유진
 * @brief 녹음기위젯에서, 채팅 블록 하나하나의 위젯 (카카오톡 말풍선)
 */
UCLASS()
class QUACKTOHELL_API UQChatBoxWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	/**
	 * @brie Text 컴포넌트 바인드
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text;
};

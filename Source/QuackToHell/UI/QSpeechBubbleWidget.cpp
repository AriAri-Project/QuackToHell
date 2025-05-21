// Copyright_Team_AriAri


#include "UI/QSpeechBubbleWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"


//void UQSpeechBubbleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
//{
//	Super::NativeTick(MyGeometry, InDeltaTime);
//
//	UpdateSpeechBubbleRotation();
//}
//
//void UQSpeechBubbleWidget::UpdateSpeechBubbleRotation()
//{
//	
//}

void UQSpeechBubbleWidget::TurnOnSpeechBubble()
{
	this->SetVisibility(ESlateVisibility::Visible);
}

void UQSpeechBubbleWidget::TurnOffSpeechBubble()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UQSpeechBubbleWidget::UpdateText(const FString& Text)
{
	TextBlock->SetText(FText::FromString(Text));
}

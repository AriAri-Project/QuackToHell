// Copyright_Team_AriAri


#include "UI/QPlayer2NSpeechBubbleWidget.h"

void UQPlayer2NSpeechBubbleWidget::TurnOnSpeechBubble()
{
	this->SetVisibility(ESlateVisibility::Visible);
}

void UQPlayer2NSpeechBubbleWidget::TurnOffSpeechBubble()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}

// Copyright_Team_AriAri


#include "UI/QLobbyWidget.h"
#include "Components/TextBlock.h"

UQLobbyWidget::UQLobbyWidget()
{
	
}

void UQLobbyWidget::UdateTextBlock(UTextBlock* TextBlock, FString NewText)
{
	TextBlock->SetText(FText::FromString(NewText));
}

void UQLobbyWidget::UdateClientNameTB()
{
}

void UQLobbyWidget::UDateHostNameTB()
{
}


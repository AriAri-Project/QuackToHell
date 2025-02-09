// Copyright_Team_AriAri


#include "UI/QInventoryWidget.h"
#include "UI/QVillageUIManager.h"
#include "QLogCategories.h"
#include "UI/QRecordWidget.h"

void UQInventoryWidget::TurnOffUI() const
{
	AQVillageUIManager::GetInstance(GetWorld())->TurnOffUI(EVillageUIType::Inventory);
}

void UQInventoryWidget::TurnOnRecordUI() const
{
	RecordWidget->SetVisibility(ESlateVisibility::Visible);
}

void UQInventoryWidget::TurnOffRecordUI() const
{
	RecordWidget->SetVisibility(ESlateVisibility::Hidden);
}

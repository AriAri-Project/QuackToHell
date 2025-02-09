// Copyright_Team_AriAri


#include "UI/QDefaultVillageWidget.h"
#include "UI/QVillageUIManager.h"

void UQDefaultVillageWidget::TurnOnMapUI()
{
	AQVillageUIManager::GetInstance(GetWorld())->TurnOnUI(EVillageUIType::Map);
}

void UQDefaultVillageWidget::TurnOnWalkieTakieUI()
{
	AQVillageUIManager::GetInstance(GetWorld())->TurnOnUI(EVillageUIType::Inventory);
}

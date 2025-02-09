// Copyright_Team_AriAri


#include "UI/QDefaultVillageWidget.h"
#include "UI/QVillageUIManager.h"
#include "QLogCategories.h"
void UQDefaultVillageWidget::TurnOnMapUI()
{
	AQVillageUIManager::GetInstance(GetWorld())->TurnOnUI(EVillageUIType::Map);
}

void UQDefaultVillageWidget::TurnOnWalkieTakieUI()
{
	AQVillageUIManager::GetInstance(GetWorld())->TurnOnUI(EVillageUIType::Inventory);
	UE_LOG(LogLogic, Log, TEXT("디폴트위젯: 브로드캐스트시도"));
	OnRecordButtonPressed.Broadcast();
}

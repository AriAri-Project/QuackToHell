// Copyright_Team_AriAri


#include "UI/QInventoryWidget.h"
#include "UI/QVillageUIManager.h"
#include "QLogCategories.h"
void UQInventoryWidget::NativeDestruct()
{
	UE_LOG(LogLogic, Log, TEXT("데이터 어떻게 오는지 확인"));

}
void UQInventoryWidget::TurnOffUI() const
{
	AQVillageUIManager::GetInstance(GetWorld())->TurnOffUI(EVillageUIType::Inventory);
}

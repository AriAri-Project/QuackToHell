// Copyright_Team_AriAri


#include "UI/QInventoryWidget.h"
#include "UI/QVillageUIManager.h"
#include "QLogCategories.h"
#include "UI/QRecordWidget.h"
#include "Components/SizeBox.h"

void UQInventoryWidget::TurnOffUI() const
{
	AQVillageUIManager::GetInstance(GetWorld())->TurnOffUI(EVillageUIType::Inventory);
}

void UQInventoryWidget::TurnOnRecordUI() const
{
	AQVillageUIManager::GetInstance(GetWorld())->TurnOnUI(EVillageUIType::Record);

	//하위컴포넌트가 없으면 하위컴포넌트로 달아주기
	if (InformationBox->GetChildrenCount()==0) {
		
		InformationBox->AddChild(AQVillageUIManager::GetInstance(GetWorld())->GetActivedWidget(EVillageUIType::Record));
	}
	
}

void UQInventoryWidget::TurnOffRecordUI() const
{
	AQVillageUIManager::GetInstance(GetWorld())->TurnOffUI(EVillageUIType::Record);
}

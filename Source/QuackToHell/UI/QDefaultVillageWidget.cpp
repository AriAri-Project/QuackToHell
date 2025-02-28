// Copyright_Team_AriAri


#include "UI/QDefaultVillageWidget.h"
#include "UI/QVillageUIManager.h"
#include "QLogCategories.h"
#include "Components/SizeBox.h"
void UQDefaultVillageWidget::BlockButtonsInteraction()
{
	UE_LOG(LogLogic, Log, TEXT("아직 미구현: UQDefaultVillageWidget::BlockButtonsInteraction"));
}
void UQDefaultVillageWidget::TurnOnTimerUI()
{
	//만약 아직 생성되지 않은 상태이면
	if (!AQVillageUIManager::GetInstance(GetWorld())->GetActivedVillageWidgets().Contains(EVillageUIType::VillageTimer)) {
		//생성하고
		AQVillageUIManager::GetInstance(GetWorld())->TurnOnUI(EVillageUIType::VillageTimer);
		//하위컴포넌트로 달아주기
		TimerBox->AddChild(AQVillageUIManager::GetInstance(GetWorld())->GetActivedVillageWidgets()[EVillageUIType::VillageTimer]);
	}
	else {
		//visible로 전환
		AQVillageUIManager::GetInstance(GetWorld())->TurnOnUI(EVillageUIType::VillageTimer);
	}
}
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

// Copyright_Team_AriAri


#include "UI/QDefaultVillageWidget.h"
#include "UI/QVillageUIManager.h"
#include "Components/Button.h"
#include "QLogCategories.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
void UQDefaultVillageWidget::BlockButtonsInteraction()
{
	//버튼 상호작용을 막는다.
	WalkieTalkieButton->SetIsEnabled(false);
	MapButton->SetIsEnabled(false);
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

void UQDefaultVillageWidget::TurnOnGrandTitle()
{
	//대제목을 보이게 한다.
	GrandTitle->SetVisibility(ESlateVisibility::Visible);
}

void UQDefaultVillageWidget::TurnOnMiddleTitle()
{
	//중제목을 보이게 한다.
	MiddleTitle->SetVisibility(ESlateVisibility::Visible);
}

void UQDefaultVillageWidget::SetGrandTitle(const FText& InText)
{
	GrandTitle->SetText(InText);
}

void UQDefaultVillageWidget::SetMiddleTitle(const FText& InText)
{
	MiddleTitle->SetText(InText);
}

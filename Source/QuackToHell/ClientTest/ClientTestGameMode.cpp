// Copyright_Team_AriAri


#include "ClientTest/ClientTestGameMode.h"
#include "UI/QCourtUIManager.h"
#include "UI/QCourtTimerWidget.h"
#include "UI/QCourtInputBoxWidget.h"
void AClientTestGameMode::BeginPlay()
{
	////유아이 띄워보기
	//AQCourtUIManager::GetInstance(GetWorld())->TurnOnUI(ECourtUIType::OpeningStatement);
	//
	////접근해보기
	//TObjectPtr<UQCourtInputBoxWidget> InputBox = Cast<UQCourtInputBoxWidget>(AQCourtUIManager::GetInstance(GetWorld())->GetActivedCourtWidgets()[ECourtUIType::InputBox]);
	//InputBox->MulticastSaveInput();

	//TObjectPtr<UQCourtTimerWidget> Timer = Cast<UQCourtTimerWidget>(AQCourtUIManager::GetInstance(GetWorld())->GetActivedCourtWidgets()[ECourtUIType::CourtTimer]);
	//Timer->UpdateServerTimeToUITime(1.f,5.f);
}

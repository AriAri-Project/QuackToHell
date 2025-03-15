// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QEvidenceExamWidget.h"
#include "UI/QCourtUIManager.h"
#include "Components/SizeBox.h"
void UQEvidenceExamWidget::TurnOnTimerUI()
{
	//만약 아직 생성되지 않은 상태이면
	if (!AQCourtUIManager::GetInstance(GetWorld())->GetActivedCourtWidgets().Contains(ECourtUIType::CourtTimer)) {
		//생성하고
		AQCourtUIManager::GetInstance(GetWorld())->TurnOnUI(ECourtUIType::CourtTimer);
		//하위컴포넌트로 달아주기
		TimerBox->AddChild(AQCourtUIManager::GetInstance(GetWorld())->GetActivedCourtWidgets()[ECourtUIType::CourtTimer]);
	}
	else {
		//visible로 전환
		AQCourtUIManager::GetInstance(GetWorld())->TurnOnUI(ECourtUIType::CourtTimer);
	}
}

void UQEvidenceExamWidget::TurnOnInputBoxUI()
{
	//만약 아직 생성되지 않은 상태이면
	if (!AQCourtUIManager::GetInstance(GetWorld())->GetActivedCourtWidgets().Contains(ECourtUIType::InputBox)) {
		//생성하고
		AQCourtUIManager::GetInstance(GetWorld())->TurnOnUI(ECourtUIType::InputBox);
		//하위컴포넌트로 달아주기
		InputBox->AddChild(AQCourtUIManager::GetInstance(GetWorld())->GetActivedCourtWidgets()[ECourtUIType::InputBox]);
	}
	else {
		//visible로 전환
		AQCourtUIManager::GetInstance(GetWorld())->TurnOnUI(ECourtUIType::InputBox);
	}
}

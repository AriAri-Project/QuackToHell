// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/QCourtInputBoxWidget.h"
#include "QLogCategories.h"
#include "QCourtUIManager.h"


void UQCourtInputBoxWidget::MulticastSaveInput_Implementation()
{
	//모두진술입력시간끝났으니입력마무리처리해라//multicastRPC
	UE_LOG(LogLogic, Log, TEXT("UQCourtInputBoxWidget::SaveInput_Implementation 미구현"));
}

void UQCourtInputBoxWidget::AlertFinishInput(FString NewText)
{
    if (UWorld* World = GetWorld())
    {
        AQCourtUIManager* UIManager = AQCourtUIManager::GetInstance(World);
        if (UIManager)
        {
            UIManager->ServerRPCAlerOpeningStatementInputEnd(true);
        }
    }
}

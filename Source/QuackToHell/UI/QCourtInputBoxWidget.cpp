// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/QCourtInputBoxWidget.h"
#include "QLogCategories.h"
#include "QCourtUIManager.h"
#include <Game/QGameInstance.h>


void UQCourtInputBoxWidget::MulticastSaveInput_Implementation()
{
	//모두진술입력시간끝났으니입력마무리처리해라//multicastRPC
	UE_LOG(LogLogic, Log, TEXT("UQCourtInputBoxWidget::SaveInput_Implementation 미구현"));
}

void UQCourtInputBoxWidget::AlertFinishInput(const FText& NewText)
{
	Cast<UQGameInstance>(GetGameInstance())->SetOpeningStetementText(NewText.ToString());
    if (GEngine)
    {
       
        GEngine->AddOnScreenDebugMessage(
            /*Key*/ -1,
            /*Time*/ 0.5f,
            /*Color*/ FColor::Yellow,
            FString::Printf(TEXT("saved text=%s"), *NewText.ToString())
        );
    }
}

// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/CourtOpeningDirection.h"
#include "UI/QCourtUIManager.h"


void UCourtOpeningDirection::AlertFinishDirection()
{
    if (UWorld* World = GetWorld())
    {
        AQCourtUIManager* UIManager = AQCourtUIManager::GetInstance(World);
        if (UIManager)
        {
            UIManager->ServerRPCAlertOpeningEnd(true);
        }
    }
}

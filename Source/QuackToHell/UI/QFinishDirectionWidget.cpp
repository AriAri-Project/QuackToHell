// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/QFinishDirectionWidget.h"
#include "Components/TextBlock.h"
#include "Game/QGameInstance.h"
#include <Game/QGameStateCourt.h>


UQFinishDirectionWidget::UQFinishDirectionWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UQFinishDirectionWidget::SetResultTexts()
{
    //1 . 플레이어 대사들 띄우기
    AQGameStateCourt* GS = GetWorld()->GetGameState<AQGameStateCourt>();

    auto OpeningStatements = GS->GetOpeningStatements();
    
    for (auto& Value : OpeningStatements) {
        if (Value.bServer) {
            ServerStatement->SetText(FText::FromString(Value.Statement));
        }
        else {
            ClientStatement->SetText(FText::FromString(Value.Statement));
        }
    }

    //2. 대사요청하기 - 피고인
    //2. 대사요청하기 - 배심원123
    //2. 대사요청하기 - 판사
}

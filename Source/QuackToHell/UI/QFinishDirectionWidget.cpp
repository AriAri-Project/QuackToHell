// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/QFinishDirectionWidget.h"
#include "Components/TextBlock.h"
#include "Game/QGameInstance.h"
#include "Character/QDefendantNPC.h"
#include "Character/QJuryNPC.h"
#include "NPCComponent.h"
#include "Character/QLaywer.h"
#include "Character/QPlayer.h"
#include <Game/QGameStateCourt.h>
#include <Player/QPlayerState.h>
#include <Kismet/GameplayStatics.h>


void UQFinishDirectionWidget::ClientRPCUpdateWidgetText_Implementation(FOpenAIResponse Response)
{
    if (Response.ConversationType == EConversationType::OpeningStatement) {
        DefendantStatement->SetText(FText::FromString(Response.ResponseText));
    }
    
}

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

    //필요변수가져오기
    UQGameInstance* GI = Cast<UQGameInstance>(GetGameInstance());
    auto Persistents = GI->GetPersistentActors();
    
    
    AQPlayer* PlayerPawn = nullptr;
    for (auto Actor : Persistents)
    {
        if (AQPlayer* Player = Cast<AQPlayer>(Actor)) {
            PlayerPawn = Player;
        }
    }

    for (auto Actor : Persistents)
    {        
        //2. 대사요청하기 - 피고인
        if (AQDefendantNPC* Defendant = Cast<AQDefendantNPC>(Actor))
        {
            FString OpeningStatementsText = GS->GetOpeningStatements()[0].Statement + GS->GetOpeningStatements()[1].Statement;
            FOpenAIRequest Request(
                Defendant->FindComponentByClass<UNPCComponent>()->GetNPCID(),
                PlayerPawn->_GetPlayerState()->GetPlayerId(),
                EConversationType::JuryFinalOpinion,
                OpeningStatementsText
            );
            Defendant->FindComponentByClass<UNPCComponent>()->TrialStatement(Request);
        }
        //2. 대사요청하기 - 배심원123
        //2. 대사요청하기 - 판사
    }
    
   
}

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
    /** @폐기 */
    // 1.플레이어 대사들 띄우기
    //AQGameStateCourt * GS = GetWorld()->GetGameState<AQGameStateCourt>();

    //auto OpeningStatements = GS->GetOpeningStatements();

    //for (auto& Value : OpeningStatements) {
    //    if (Value.bServer) {
    //        ServerStatement->SetText(FText::FromString(Value.Statement));
    //    }
    //    else {
    //        ClientStatement->SetText(FText::FromString(Value.Statement));
    //    }
    //}

    ////피고인 대사 띄우기
    //if (Response.ConversationType == EConversationType::OpeningStatement) {
    //    DefendantStatement->SetText(FText::FromString(Response.ResponseText));
    //}
    //
}

UQFinishDirectionWidget::UQFinishDirectionWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UQFinishDirectionWidget::SetResultTexts()
{
    
    // 1.플레이어 대사들 띄우기
    AQGameStateCourt * GS = GetWorld()->GetGameState<AQGameStateCourt>();

    auto OpeningStatements = GS->GetOpeningStatements();

    for (auto& Value : OpeningStatements) {
        if (Value.bServer) {
            ServerStatement->SetText(FText::FromString(Value.Statement));
        }
        else {
            ClientStatement->SetText(FText::FromString(Value.Statement));
        }
    }
    // 2. 피고인 대사 띄우기
    if (DefendantStatementsRandList.Num() > 0)
    {
        int32 Idx = FMath::RandRange(0, DefendantStatementsRandList.Num() - 1);
        DefendantStatement->SetText(DefendantStatementsRandList[Idx]);
    }

    // 3. 배심원 대사 띄우기 (중복 없이 3개)
    const int32 JuryCount = JuryStatementsRandList.Num();
    if (JuryCount > 0)
    {
        int32 I1 = FMath::RandRange(0, JuryCount - 1);

        int32 I2 = FMath::RandRange(0, JuryCount - 1);
        while (I2 == I1 && JuryCount > 1)
        {
            I2 = FMath::RandRange(0, JuryCount - 1);
        }

        int32 I3 = FMath::RandRange(0, JuryCount - 1);
        while ((I3 == I1 || I3 == I2) && JuryCount > 2)
        {
            I3 = FMath::RandRange(0, JuryCount - 1);
        }

        Jury1Statement->SetText(JuryStatementsRandList[I1]);
        Jury2Statement->SetText(JuryStatementsRandList[I2]);
        Jury3Statement->SetText(JuryStatementsRandList[I3]);
    }

    // 4. 판사 대사 띄우기
    if (JudgeStatementsRandList.Num() > 0)
    {
        int32 JIdx = FMath::RandRange(0, JudgeStatementsRandList.Num() - 1);
        JudgeStatement->SetText(JudgeStatementsRandList[JIdx]);
    }
}

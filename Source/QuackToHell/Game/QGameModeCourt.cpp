// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/QGameModeCourt.h"
#include "Game/QGameStateCourt.h"


void AQGameModeCourt::StartPlay()
{
    Super::StartPlay();

    
}

AQGameModeCourt::AQGameModeCourt()
    :Super()
{
}

void AQGameModeCourt::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AQGameModeCourt::BeginPlay()
{
	Super::BeginPlay();
}

void AQGameModeCourt::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (!HasAuthority())
        return;

    AQGameStateCourt* GS = GetGameState<AQGameStateCourt>();
    if (!GS)
        return;

    // 3초 뒤에 Opening 연출 시작
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(
        TimerHandle,
        FTimerDelegate::CreateLambda([GS]()
            {
                GS->MultiRPCStartDirection(ECourtDirectionType::Opening);
            }),
        15.0f,   // 딜레이 (초)
        false   // 반복 안 함
    );

}

void AQGameModeCourt::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/QCourtTimerWidget.h"
#include "Components/TextBlock.h"
#include "QLogCategories.h"
#include "QCourtUIManager.h"

static float InternalAccum = 0.0f;



void UQCourtTimerWidget::UpdateServerTimeToUITime(float DeltaTime, const float MaxTime)
{
    if (bHasCalledFinishTimer) {
        return;
    }
    if (InternalAccum >= MaxTime) {
        bHasCalledFinishTimer = true;
        //서버RPC호출
        if (UWorld* World = GetWorld())
        {
            AQCourtUIManager* UIManager = AQCourtUIManager::GetInstance(World);
            if (UIManager)
            {
                UIManager->ServerRPCAlertOpeningStatementPerformEnd(true);
            }
        }
    }

    // 1) 내부 누적 변수에 델타를 계속 더한다.
    InternalAccum += DeltaTime;

    // 2) 최대값을 넘어가지 않도록 클램프
    if (InternalAccum > MaxTime)
    {
        InternalAccum = MaxTime;
    }

    // 3) 남은 시간 계산
    float Remaining = FMath::Clamp(MaxTime - InternalAccum, 0.0f, MaxTime);
    int32 TotalSec = FMath::CeilToInt(Remaining);
    int32 Min = TotalSec / 60;
    int32 Sec = TotalSec % 60;

    // 4) "MM:SS" 문자열 생성
    FString TimeStr = FString::Printf(TEXT("%02d:%02d"), Min, Sec);

    // 5) 텍스트박스에 반영
    if (TextBox)
    {
        TextBox->SetText(FText::FromString(TimeStr));
    }
}

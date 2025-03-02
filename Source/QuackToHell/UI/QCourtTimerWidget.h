// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QCourtTimerWidget.generated.h"

/**
 * @author 전유진
 * @brief 재판장 타이머 위젯 클래스
 */
UCLASS()
class QUACKTOHELL_API UQCourtTimerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/**
	 * @brief 서버에서 누적된 시간을 넣어주면, 함수 내에서 정규화된(0~1사이의값) 값으로 변경한다.
	 *
	 * @param AccumulatedTime: 0부터 시작해 얼마나 시간이 누적되었는지 전달
	 * @param MaxTime: 타이머가 울려야되는 max값을 전달 (ex. 840초면 타이머 종료)
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateServerTimeToUITime(float AccumulatedTime, const float MaxTime);
};

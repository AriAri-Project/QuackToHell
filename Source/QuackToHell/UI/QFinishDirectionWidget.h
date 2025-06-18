// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QFinishDirectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API UQFinishDirectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 생성자 선언
	UQFinishDirectionWidget(const FObjectInitializer& ObjectInitializer);
public:
	UFUNCTION(BlueprintCallable) 
	void SetResultTexts();
private:
    // ----- 여기부터 바인딩할 텍스트 블록 멤버들 -----
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ServerStatement;            // [서버대사] “test”

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ClientStatement;            // [클라대사] “test”

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* DefendantStatement;          // [피고인대사] “test”

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Jury1Statement;         // [배심원2대사] “test”
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Jury2Statement;         // [배심원2대사] “test”
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Jury3Statement;         // [배심원2대사] “test”

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* JudgeStatement;            // [판사대사] “test”
    // ---------------------------------------

};

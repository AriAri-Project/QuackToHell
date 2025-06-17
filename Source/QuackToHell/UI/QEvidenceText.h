// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "QLogCategories.h"
#include "QEvidenceText.generated.h"

/**
 * @author 전유진
 */
UCLASS()
class QUACKTOHELL_API UQEvidenceText : public UUserWidget
{
	GENERATED_BODY()
protected:
    // UMG Designer에서 만든 TextBlock 컴포넌트를 이 이름으로 바인딩
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* EvidenceTextBlock;

public:
    /**
     * @brief 설명 문자열을 화면에 띄웁니다.
     */
    UFUNCTION(BlueprintCallable, Category = "Evidence")
    void SetText(const FString& InDescription)
    {
        if (!EvidenceTextBlock)
        {
            UE_LOG(LogLogic, Error, TEXT("UQEvidenceText::SetText - EvidenceTextBlock is null"));
            return;
        }
        EvidenceTextBlock->SetText(FText::FromString(InDescription));
    }
};

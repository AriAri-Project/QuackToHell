// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "QLogCategories.h"
#include "QEvidenceImage.generated.h"

/**
 * @author 전유진
 */
UCLASS()
class QUACKTOHELL_API UQEvidenceImage : public UUserWidget
{
	GENERATED_BODY()
protected:
    // UMG Designer에서 만든 Image 컴포넌트를 이 이름으로 바인딩
    UPROPERTY(meta = (BindWidget))
    class UImage* EvidenceImage;

public:
    /**
     * @brief Path에 해당하는 Texture2D를 로드해서 이미지를 세팅합니다.
     * @param ImageAssetPath "/Game/Textures/T_MyEvidence.T_MyEvidence" 식으로 넘기세요.
     */
    UFUNCTION(BlueprintCallable, Category = "Evidence")
    void SetImage(const FString& ImageAssetPath)
    {
        if (!EvidenceImage)
        {
            UE_LOG(LogLogic, Error, TEXT("UQEvidenceImage::SetImage - EvidenceImage is null"));
            return;
        }

        UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *ImageAssetPath);
        if (!Texture)
        {
            UE_LOG(LogLogic, Warning, TEXT("UQEvidenceImage::SetImage - Failed to load texture at %s"), *ImageAssetPath);
            return;
        }

        // 이미지 브러시에 텍스처를 설정
        EvidenceImage->SetBrushFromTexture(Texture, /* bMatchSize = */ true);
    }
};

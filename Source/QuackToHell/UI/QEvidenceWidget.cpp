// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/QEvidenceWidget.h"
#include "QLogCategories.h"
#include "UI/QVillageUIManager.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "UI/QDefaultVillageWidget.h"
#include "UI/QEvidenceText.h"
#include "UI/QEvidenceImage.h"
#include "Blueprint/UserWidget.h"



void UQEvidenceWidget::NativeConstruct()
{
    /*델리게이트 바인딩*/
    TObjectPtr<UQDefaultVillageWidget> DefaultVillageWidet = Cast<UQDefaultVillageWidget>((AQVillageUIManager::GetInstance(GetWorld()))->GetActivedWidget(EVillageUIType::DefaultVillageUI));
    DefaultVillageWidet->OnEvidenceButtonPressed.AddDynamic(this, &UQEvidenceWidget::UpdateInventory);
    /* 버튼 정보 가져오기 */
    /*버튼 정보 가져오기*/
    for (int8 i = 0; i < EvidenceMaxNum; i++) {
        /*FString포맷팅(출력아님)*/
        FString ImageName = FString::Printf(TEXT("Image_%d"), i);
        FString EvidenceName = FString::Printf(TEXT("Name_%d"), i);

        /*하위위젯가져오기 (GetWidgetFromName의 반환값은 UserWidget)*/
        TObjectPtr<UImage> Image = Cast<UImage>(GetWidgetFromName(*ImageName));
        TObjectPtr<UTextBlock> Name = Cast<UTextBlock>(GetWidgetFromName(*EvidenceName));

		FEvidenceButton Temp;
        Temp.EvidenceName = Name;
		Temp.Image = Image;

		if (Image && Name) {
            Buttons.Add(Temp);
		}
		else {
			UE_LOG(LogLogic, Error, TEXT("UQEvidenceWidget::NativeConstruct - Image or Name not found for index %d"), i);
		}
    }
}

UQEvidenceWidget::UQEvidenceWidget(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    /*ChatBox 위젯 블루프린트 클래스정보 가져오기*/
    static ConstructorHelpers::FClassFinder<UQEvidenceImage> ImageBoxWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QEvidenceImage.WBP_QEvidenceImage_C'"));
    if (ImageBoxWidgetAsset.Succeeded()) {
        ClassOfImageBox = ImageBoxWidgetAsset.Class;
    }
    
    static ConstructorHelpers::FClassFinder<UQEvidenceImage> TextBoxWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_EvidenceText.WBP_EvidenceText_C'"));
    if (TextBoxWidgetAsset.Succeeded()) {
        ClassOfTextBox = TextBoxWidgetAsset.Class;
    }
}

void UQEvidenceWidget::ShowEvidenceInfo(int32 IndexNum) const
{
    if (!VerticalBox)
    {
        UE_LOG(LogLogic, Error, TEXT("UQEvidenceWidget::ShowEvidenceInfo - VerticalBox is null"));
        return;
    }

    if (!EvidenceList.IsValidIndex(IndexNum))
    {
        UE_LOG(LogLogic, Warning, TEXT("UQEvidenceWidget::ShowEvidenceInfo - Invalid index %d"), IndexNum);
        return;
    }

    // 1) Clear out previous children
    VerticalBox->ClearChildren();

    const FEvidence& Evidence = EvidenceList[IndexNum];

	// 2) Create and add Image widget
	if (ClassOfImageBox)
	{
		UQEvidenceImage* ImageWidget = CreateWidget<UQEvidenceImage>(GetWorld(), ClassOfImageBox);
		ImageWidget->SetImage(Evidence.GetImagePath()); // Assuming SetImage is a method in UQEvidenceImage to set the image path
        if (ImageWidget)
		{
			VerticalBox->AddChildToVerticalBox(ImageWidget);
		}
		else
		{
			UE_LOG(LogLogic, Error, TEXT("UQEvidenceWidget::ShowEvidenceInfo - Failed to create ImageWidget"));
		}
	}
	// 3) Create and add Text widget
	if (ClassOfTextBox)
	{
		UQEvidenceText* TextWidget = CreateWidget<UQEvidenceText>(GetWorld(), ClassOfTextBox);
		TextWidget->SetText(Evidence.GetDescription()); // Assuming SetText is a method in UQEvidenceText to set the text
        if (TextWidget)
		{
			VerticalBox->AddChildToVerticalBox(TextWidget);
		}
		else
		{
			UE_LOG(LogLogic, Error, TEXT("UQEvidenceWidget::ShowEvidenceInfo - Failed to create TextWidget"));
		}
	}
}

void UQEvidenceWidget::AddEvidence(const FEvidence& InputEvidence)
{
    // 이미 있는 ID인지 검사…
    for (const auto& E : EvidenceList)
    {
        if (E.GetID() == InputEvidence.GetID())
        {
            return;
        }
    }
    // 배열에 복사 삽입
    EvidenceList.Add(InputEvidence);
}

void UQEvidenceWidget::UpdateInventory()
{
    // EvidenceList와 Buttons 배열 크기만큼 반복
    for (int32 i = 0; i < EvidenceList.Num() && i < Buttons.Num(); ++i)
    {
        const FEvidence& Evidence = EvidenceList[i];
        FEvidenceButton& BtnData = Buttons[i];

        if (BtnData.Image && BtnData.EvidenceName)
        {
            // 1) 이미지 로드
            //    GetImagePath() 에는 "/Game/Textures/T_MyEvidence.T_MyEvidence" 같은 에셋 경로를 넘기셔야 합니다.
            const FString& Path = Evidence.GetImagePath();
            UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *Path);
            if (Texture)
            {
                // 브러시로 설정 (굳이 사이즈 자동 맞춤하려면 두 번째 인자를 true)
                BtnData.Image->SetBrushFromTexture(Texture, /* bMatchSize = */ true);
            }
            else
            {
                UE_LOG(LogLogic, Error,
                    TEXT("UQEvidenceWidget::UpdateInventory - Failed to load texture at path '%s'"),
                    *Path);
            }

            // 2) 이름 설정
            BtnData.EvidenceName->SetText(FText::FromString(Evidence.GetName()));
        }
        else
        {
            UE_LOG(LogLogic, Error,
                TEXT("UQEvidenceWidget::UpdateInventory - Widget components missing at index %d"),
                i);
        }
    }
}

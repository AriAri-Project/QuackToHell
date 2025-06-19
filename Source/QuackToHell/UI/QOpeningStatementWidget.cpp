// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QOpeningStatementWidget.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Game/QGameInstance.h"             // 여러분의 GameInstance 클래스
#include "GameData/QEvidenceData.h"    // FEvidence 정의

// 파일 I/O
#include "HAL/FileManager.h"               // FPaths::FileExists
#include "Misc/FileHelper.h"               // FFileHelper::LoadFileToArray
#include "Misc/Paths.h"                    // FPaths

// 이미지 디코딩
#include "IImageWrapper.h"                 // IImageWrapper
#include "IImageWrapperModule.h"           // IImageWrapperModule
#include "Modules/ModuleManager.h"         // FModuleManager

// 런타임 텍스처 생성
#include "Engine/Texture2D.h"               // UTexture2D

#include "UI/QCourtUIManager.h"
#include <Kismet/KismetRenderingLibrary.h>
void UQOpeningStatementWidget::PopulateEvidenceGrid()
{
    if (!EvidenceGrid) return;

    UQGameInstance* GI = Cast<UQGameInstance>(GetWorld()->GetGameInstance());
    if (!GI) return;

    const TArray<FEvidence>& Inv = GI->GetInventoryEvidences();
    EvidenceGrid->ClearChildren();

    //const int32 Columns = 5;
    //for (int32 i = 0; i < Inv.Num(); ++i)
    //{
    //    const FEvidence& E = Inv[i];
    //    UTexture2D* Tex = nullptr;

    //    // 1) 에디터에 임포트된 에셋 우선 시도
    //    Tex = Cast<UTexture2D>(
    //        StaticLoadObject(UTexture2D::StaticClass(), nullptr, *E.GetImagePath())
    //    );

    //    // 2) 디스크 경로가 실제 PNG 파일이면, 바이트 배열 읽어와서
    //    if (!Tex && FPaths::FileExists(E.GetImagePath()))
    //    {
    //        TArray<uint8> FileData;
    //        if (FFileHelper::LoadFileToArray(FileData, *E.GetImagePath()))
    //        {
    //            // 한 줄로 디스크 버퍼 → 런타임 텍스처
    //            Tex = UKismetRenderingLibrary::ImportBufferAsTexture2D(this, FileData);
    //        }
    //    }

    //    // 3) 위젯 생성 & 브러시 설정
    //    UImage* Img = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
    //    if (Img && Tex)
    //    {
    //        Img->SetBrushFromTexture(Tex);
    //    }

    //    // 4) 그리드에 추가
    //    int32 Row = i / Columns;
    //    int32 Col = i % Columns;
    //    EvidenceGrid->AddChildToUniformGrid(Img, Row, Col);
    //}
}
void UQOpeningStatementWidget::TurnOnTimerUI()
{
	//만약 아직 생성되지 않은 상태이면
	if (!TimerBox->HasAnyChildren()) {
		
		//하위컴포넌트로 달아주기
		TimerBox->AddChild(AQCourtUIManager::GetInstance(GetWorld())->GetActivedCourtWidgets(ECourtUIType::CourtTimer));
	}
	else {
		//visible로 전환
		AQCourtUIManager::GetInstance(GetWorld())->TurnOnUI(ECourtUIType::CourtTimer);
	}
}

void UQOpeningStatementWidget::TurnOnInputBoxUI()
{
	//만약 아직 생성되지 않은 상태이면
	if (!InputBox->HasAnyChildren()) {
		
		//하위컴포넌트로 달아주기
		InputBox->AddChild(AQCourtUIManager::GetInstance(GetWorld())->GetActivedCourtWidgets(ECourtUIType::InputBox));
	}
	else {
		//visible로 전환
		AQCourtUIManager::GetInstance(GetWorld())->TurnOnUI(ECourtUIType::InputBox);
	}
}

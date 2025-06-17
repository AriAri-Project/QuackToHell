// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Actors/AQEvidenceActor.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "QEvidenceWidget.generated.h"

/**
 * @brief 버튼의 정보를 가지는 구조체입니다.
 */
USTRUCT()
struct FEvidenceButton {
	GENERATED_BODY()
private:
	UPROPERTY()
	TObjectPtr<class UImage> Image;

	UPROPERTY()
	TObjectPtr<class UTextBlock> EvidenceName;

	//해당 클래스에서만 접근가능하게.
	friend class UQEvidenceWidget;
};

/**
 * @author 전유진
 * @brief 증거물 인벤토리
 */
UCLASS()
class QUACKTOHELL_API UQEvidenceWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
public:
	UQEvidenceWidget(const FObjectInitializer& ObjectInitializer);
public:
	/**
	 * @brief 클릭 시 정보띄우게, 블루프린트 노드연결 할 수 잇게 열어줌.
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "Evidence")
	void ShowEvidenceInfo(int32 IndexNum) const;
	
	/**
	 * @brief 증거물 세팅.
	 * 
	 * @param InputEvidence
	 */
	void AddEvidence(const FEvidence& InputEvidence);
private:
	/**
	 * @brief 증거물 데이터.
	 */
	TArray<FEvidence> EvidenceList;
private:
	/**
	 * @brief 해당 위젯의 하위컴포넌트인 버튼들의 정보를 갖고있음.
	 */
	UPROPERTY()
	TArray<FEvidenceButton > Buttons;
	const int32 EvidenceMaxNum = 8;
private:
	/**
	 * @brie Vertical Box 컴포넌트 바인드
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> VerticalBox;
private:
	/**
	 * @brief 대화기록을 업데이트합니다.
	 * 업데이트시점: 녹음기버튼이 눌렸을 때
	 *
	 */
	UFUNCTION()
	void UpdateInventory();
private:
	/**
	 * @brief 이미지설명 WBP 클래스정보.
	 */
	TSubclassOf<class UQEvidenceImage> ClassOfImageBox;
	/**
	 * @brief 텍스트설명 WBP 클래스정보.
	 */
	TSubclassOf<class UQEvidenceText> ClassOfTextBox;

	
};





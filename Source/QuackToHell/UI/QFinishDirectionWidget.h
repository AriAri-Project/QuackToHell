// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCComponent.h"
#include "QFinishDirectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API UQFinishDirectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(Client,Reliable)
    void ClientRPCUpdateWidgetText(FOpenAIResponse Response);
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
private:
    // 판사 대사 리스트
    UPROPERTY()
    TArray<FText> JudgeStatementsRandList = {
        FText::FromString(TEXT("공판에 제시된 증거와 양측의 논거를 심층적으로 검토한 결과, 피고에게 무죄를 선고합니다.")),
        FText::FromString(TEXT("검찰의 주장과 변호인의 반박, 그리고 제시된 모든 증언을 고려하여, 이 법정은 무죄를 선언합니다.")),
        FText::FromString(TEXT("각 당사자가 제시한 증거와 진술을 전체적으로 평가한 결과, 피고인의 무죄를 선포합니다.")),
        FText::FromString(TEXT("감찰 자료와 증언, 공판 기록 전반을 종합 분석한 끝에, 피고의 무죄를 인정합니다.")),
        FText::FromString(TEXT("공판 절차에서 제시된 물증과 증언, 각 측의 주장을 균형 있게 검토하여 무죄를 선고합니다.")),
        FText::FromString(TEXT("증거 목록과 현장 조사 자료, 공판에서 오간 의견을 전부 반영하여 피고에게 무죄 판결을 내립니다.")),
        FText::FromString(TEXT("모든 증언과 문서 증거, 당사자들의 의견을 고려한 심리 끝에, 무죄를 선언합니다.")),
        FText::FromString(TEXT("검찰과 변호인이 제시한 논리와 증언, 그리고 사건 기록 전체를 종합하여 무죄를 선포합니다.")),
        FText::FromString(TEXT("공판에서 드러난 사실관계와 증거의 신빙성을 면밀히 분석한 결과, 피고인의 무죄를 인정합니다.")),
        FText::FromString(TEXT("각종 증언과 증거, 논점이 된 쟁점을 전부 고려하여 피고에게 무죄를 선고합니다."))
    };

    // 배심원 대사 리스트
    UPROPERTY()
    TArray<FText> JuryStatementsRandList = {
        FText::FromString(TEXT("저는 증거와 증언을 종합해 볼 때, 피고의 진술은 일관성이 부족하다고 생각합니다.")),
        FText::FromString(TEXT("피고인의 태도에서 진정성이 느껴졌기에, 무죄 추정의 원칙을 존중하고 싶습니다.")),
        FText::FromString(TEXT("합당한 근거가 부족하지만, 일말의 의문도 남기기 어려워 보입니다.")),
        FText::FromString(TEXT("제게 가장 중요했던 것은 증거물의 신뢰성입니다. 이를 기준으로 판단하겠습니다.")),
        FText::FromString(TEXT("피고 측 증언이 감정에 치우쳤다고 보았기에, 검찰 측 주장을 더 신뢰합니다.")),
        FText::FromString(TEXT("검찰과 변호인 양쪽 논리를 들었지만, 증거물의 객관성이 중시되어야 한다고 생각합니다.")),
        FText::FromString(TEXT("피고의 진술에 동정심은 들지만, 법 앞에서는 사실만으로 판단해야 합니다.")),
        FText::FromString(TEXT("제게 주어진 결정권은 공정한 시각에서 책임을 다하는 것입니다. 편견 없이 심리하겠습니다.")),
        FText::FromString(TEXT("판단에 앞서, 피고인의 과거 이력을 참고했을 때 중요한 불일치는 발견되지 않았습니다.")),
        FText::FromString(TEXT("이 사건의 핵심은 ‘의도’라고 생각합니다. 의도 여부를 꼼꼼히 살펴보겠습니다."))
    };

    // 피고인 대사 리스트
    UPROPERTY()
    TArray<FText> DefendantStatementsRandList = {
        FText::FromString(TEXT("존경하는 배심원님들, 저는 이 사건이 실수였음을 간절히 호소드립니다.")),
        FText::FromString(TEXT("제 의도가 결코 악의가 아니었음을 증명하고자 합니다.")),
        FText::FromString(TEXT("증거와 달리, 저는 피해자를 보호하려던 행동이었습니다.")),
        FText::FromString(TEXT("당시 상황을 되짚어보면, 모든 선택이 최선이었다는 점을 설명드리고 싶습니다.")),
        FText::FromString(TEXT("제 과실이나 오해가 불러온 해프닝이니, 관대한 판단을 부탁드립니다.")),
        FText::FromString(TEXT("평소 모범시민으로 살아왔으며, 이 한 번의 실수가 전부임을 알아주십시오.")),
        FText::FromString(TEXT("지금까지 쌓아온 신뢰를 믿고, 오늘 제 진실을 들어주시면 감사하겠습니다.")),
        FText::FromString(TEXT("두려운 마음으로 이 자리에 섰지만, 진심으로 선처를 구합니다.")),
        FText::FromString(TEXT("모든 증언이 왜곡되었으니, 제 입장을 왜곡 없이 들어주시길 바랍니다.")),
        FText::FromString(TEXT("제 행위가 잘못됐다면 응당 책임을 지겠습니다. 다만 순수한 의도였음을 이해해주십시오."))
    };
};

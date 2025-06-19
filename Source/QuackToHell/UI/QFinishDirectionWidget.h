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
        FText::FromString(TEXT("배심원 전원의 평결을 존중하며, ‘피고인의 행위에 의도가 결여되었다’는 의견을 수용하여 무죄를 선고합니다.")),
        FText::FromString(TEXT("배심원들이 ‘증거의 신빙성이 부족하다’고 판단한 만큼, 피고인을 천국으로 보내는 것이 합당하다고 봅니다.")),
        FText::FromString(TEXT("배심원 성원들의 ‘피고인의 진술에 진정성이 있다’는 소견을 반영하여, 이 법정은 무죄를 선언합니다.")),
        FText::FromString(TEXT("배심원들이 ‘피고의 의도가 선의였다’고 만장일치로 확인했기에, 피고인을 지옥이 아닌 천국으로 이끕니다.")),
        FText::FromString(TEXT("전원합의체 배심원들이 ‘증거가 충분치 않다’고 평결했으니, 피고인을 무죄로 석방합니다.")),
        FText::FromString(TEXT("배심원 의견을 종합한 결과 ‘피고 측 주장에 일말의 의문도 없다’는 결론이므로 무죄를 선포합니다.")),
        FText::FromString(TEXT("배심원들이 ‘행위의 정당성이 인정된다’고 판정했으므로, 피고인은 지옥이 아닌 천국으로 향할 자격이 있습니다.")),
        FText::FromString(TEXT("배심원 전원의 만장일치 평결을 토대로, 피고인을 무죄로 인정하여 천국으로 보내기로 판결합니다.")),
        FText::FromString(TEXT("배심원들이 ‘피고인의 태도와 진술이 신뢰할 만하다’고 의견을 모았으니, 이 법정은 무죄를 선고합니다.")),
        FText::FromString(TEXT("배심원들의 ‘증거 불충분’ 의견에 따라, 피고인을 지옥 대신 천국으로 인도하는 판결을 내립니다."))
    };

    // 배심원 대사 리스트
    UPROPERTY()
    TArray<FText> JuryStatementsRandList = {
        FText::FromString(TEXT("저는 증거와 증언을 종합해 볼 때, 피고의 진술은 일관성이 부족하다고 생각합니다.")),
        FText::FromString(TEXT("피고인의 태도에서 진정성이 느껴졌기에, 무죄 추정 원칙을 존중하고 싶습니다.")),
        FText::FromString(TEXT("합당한 근거가 부족하지만, 일말의 의문도 남기기 어렵다고 판단됩니다.")),
        FText::FromString(TEXT("제게 가장 중요했던 것은 증거물의 신뢰성입니다. 이를 기준으로 판단하겠습니다.")),
        FText::FromString(TEXT("피고 측 증언이 감정에 치우쳐 있다고 보았기에, 검찰 측 주장을 더 신뢰합니다.")),
        FText::FromString(TEXT("검찰과 변호인 양쪽의 논리를 모두 들었지만, 증거물의 객관성이 중시되어야 합니다.")),
        FText::FromString(TEXT("피고의 진술에 동정심은 들지만, 법 앞에서는 사실만으로 판단해야 합니다.")),
        FText::FromString(TEXT("제게 준 결정권은 공정한 시각에서 책임을 다하는 것입니다. 편견 없이 심리하겠습니다.")),
        FText::FromString(TEXT("판단에 앞서, 피고인의 과거 이력을 참고했을 때 큰 불일치가 없음을 확인했습니다.")),
        FText::FromString(TEXT("이 사건의 핵심은 ‘의도’라고 생각합니다. 의도가 범죄로 귀결되었는지 꼼꼼히 따져보겠습니다."))
    };

    // 피고인 대사 리스트
    UPROPERTY()
    TArray<FText> DefendantStatementsRandList = {
        FText::FromString(TEXT("존경하는 배심원님들, 저는 이 사건이 실수였음을 간절히 호소드립니다.")),
        FText::FromString(TEXT("제 의도가 결코 악의가 아니었음을 오늘 이 자리에서 증명하겠습니다.")),
        FText::FromString(TEXT("증거에 나타난 사실과 달리, 저는 피해자를 보호하려던 행동이었습니다.")),
        FText::FromString(TEXT("당시 상황을 다시 떠올리면, 제 모든 선택이 최선이었음을 설명드리고 싶습니다.")),
        FText::FromString(TEXT("제 과실이나 오해가 불러온 해프닝이니, 관대한 판단을 부탁드립니다.")),
        FText::FromString(TEXT("저는 평소 모범시민으로 살아왔으며, 이 한 번의 실수가 전부입니다.")),
        FText::FromString(TEXT("지금까지 제게 쌓인 신뢰를 믿고, 오늘 진실을 들어주시면 감사하겠습니다.")),
        FText::FromString(TEXT("두려운 심정으로 이 자리에 섰지만, 진심으로 선처를 구합니다.")),
        FText::FromString(TEXT("모든 증언이 왜곡되었고, 제 입장을 왜곡 없이 들어주길 바랍니다.")),
        FText::FromString(TEXT("저의 행위가 잘못됐다면 다그침을 달게 받겠습니다. 다만 의도가 순수했음을 이해해주시길 바랍니다."))
    };

};

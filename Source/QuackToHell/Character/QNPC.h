// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Character/QCharacter.h"
#include "GameData/QConversationData.h"
#include "Public/NPCComponent.h"
#include "QNPC.generated.h"

/**
 * @author 전유진 유서현
 * @brief NPC 캐릭터 클래스입니다.
 */
UCLASS()
class QUACKTOHELL_API AQNPC : public AQCharacter
{
	GENERATED_BODY()

public:
	AQNPC(const FObjectInitializer& ObjectInitializer);
	/**
	 * @brief 스피치버블 위젯을 리턴합니다. NPCController에서 접근하기 위함입니다.
	 * @return speechbubblewidget
	 */
	class UQSpeechBubbleWidget* GetSpeechBubbleWidget() const;

public:
	// N2N
	FTimerHandle N2NTimerHandle;
	bool N2NCoolTimeCharged = false;
	
	void CountDownN2N();
	bool CheckCanStartConversN2N();
	bool CheckCanFinishConversN2N();
	
	void RequestConversationN2N(AQNPC* TargetNPC);
	void StartConversationN2N(AQNPC* TargetNPC, FOpenAIResponse FirstResponse);
	void ReplyConversationN2N(AQNPC* TargetNPC, FOpenAIResponse ReplyResponse);
	void FinishConversationN2N();
	
	// NMono
	const int32 NMonoCoolTimeInit = 80;
	int32 NMonoCoolTime = NMonoCoolTimeInit;
	FTimerHandle NMonoTimerHandle;

	void CountDownNMonologue();
	void RequestNMonologueText();
	void StartNMonologue(FOpenAIResponse Monologue);
	void FinishNMonologue();
protected:
	/**
	 * @brief Sphere 컴포넌트입니다. 플레이어를 기준으로 원형을 그려 트리거를 탐지합니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<class USphereComponent> InteractionSphereComponent;
	/**
	 * @brief Sphere 컴포넌트의 반지름입니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interaction")
	float SphereRadius = 200.f;
protected:
	virtual void BeginPlay() override;
protected:
	/** @brief NPCComponent를 멤버변수로 갖습니다 */
	UPROPERTY()
	TObjectPtr<class UNPCComponent> NPCComponent;
protected:
	/**
	 * @brief 
	 * 말풍선 UI를 붙일 컴포넌트 
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent> SpeechBubbleWidgetComponent;


private:
	/** @brief 런타임에 캐스트해 쓸 실제 위젯: 말풍선  */
	UPROPERTY()
	class UQSpeechBubbleWidget* SpeechBubbleWidget;
	
private:
	// NPC 대화
	UPROPERTY(Replicated)
	bool bCanStartConversN2N = false;

	UPROPERTY(Replicated)
	bool bCanFinishConversN2N = false;

	UPROPERTY(Replicated)
	EConversationType NPCConversationState = EConversationType::None;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Reliable)
	void ServerRPCCanCanStartConversN2N(const AQNPC* NPC);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCCanCanFinishConversN2N(const AQNPC* NPC);
private:
	/*충돌처리*/
	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	EConversationType GetNPCConversationState() const
	{
		return NPCConversationState;
	}

	void SetNPCConversationState(EConversationType NewState)
	{
		NPCConversationState = NewState;
	}
};

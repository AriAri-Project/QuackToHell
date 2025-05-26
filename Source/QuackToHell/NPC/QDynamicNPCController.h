// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "NPC/QNPCController.h"
#include "NPCComponent.h"
#include "QDynamicNPCController.generated.h"

/**
 * @author 전유진
 * @brief 스스로 움직이고, 역동적으로 상호작용(n2n, p2n)하는 Dynamic NPC 컨트롤러입니다.  
 */
UCLASS()
class QUACKTOHELL_API AQDynamicNPCController : public AQNPCController
{
	GENERATED_BODY()
public:
    /**
     * @brief N2N, NMonolog상황에서 호출됩니다. 
	 * 말풍선을 켜고, 대사 내용을 넣습니다.
	 * @param Text 대사 내용
     */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowSpeechBubbleWithText(const FString& Text) const;
    /**
	 * @brief N2N, NMonolog상황에서 호출됩니다. 
	 * 말풍선을 끕니다.
     */
    UFUNCTION(NetMulticast, Reliable)
    void MulticastTurnOffSpeechBubble();

public:
    /**
     * @brief. 얼음땡
     */
    void UnFreezePawn();
    /**
     * @brief NPC와의 대화시작 요청을 처리하는 함수입니다. npc-npc, player-npc 대화시스템에 활용됩니다.
     * @param 상대방의 폰 정보를 넘깁니다.
     */
    void StartDialog(TObjectPtr<APawn> Pawn, ENPCConversationType ConversationType);
    /** @brief NPC의 몸을 멈춥니다. */
    void FreezePawn();
    /** @brief 상대방을 향해 고개를 회전합니다. */
    void RotateToOpponent(const TObjectPtr<APawn> MyPawn);
    /**
    * @brief NPC와의 대화중단 요청을 처리하는 함수입니다. npc-npc, player-npc 대화시스템에 활용됩니다.
    *
    */
    void EndDialog();
    /**
     * @brief 입력에 대해 대답하라는 요구를 처리합니다.
     * @param query을 인자로 넣습니다.
     */
    //void Response(FString& Text, EConversationType ConversationType);
    ///** @brief 응답완료의 처리를 위해 델리게이트를 public으로 열어놓습니다. 응답완료 시 호출될 함수를 추가하세요 */
    //UPROPERTY()
    //FResponseDelegate OnResponseFinished;
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
protected:
    /** @brief behavior tree를 지정합니다. */
    UPROPERTY(EditAnyWhere, Category = "NPCBehavior")
    TObjectPtr<UBehaviorTree> BehaviorTree;
    /** @brief blackboard를 지정합니다. */
    UPROPERTY()
    //GC관리를 위해 UPROPERTY()합니다.
    UBlackboardComponent* BlackboardComp;
    /** @brief NPCComponent를 멤버변수로 가집니다 */
    TObjectPtr<class UNPCComponent> NPCComponent;
private:

    /** @brief VillageUIManager정보를 갖습니다. */
    TObjectPtr<class AQVillageUIManager> VillageUIManager;

    /**
     * @brief 상대방 정보.
     */
    TObjectPtr<APawn> OpponentPawn;
    /**
     * @brief 회전시작 시 on, 회전 끝날 시 off.
     */
    bool bIsRotating = false;
    /**
     * @brief 상대방을 향해 회전량 업데이트 함수.
     * @param 상대방의 pawn
     */
    void UpdateRotation();


private:
    TObjectPtr<class AQNPC> MyPawn;
};

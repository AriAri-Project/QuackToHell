// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "QNPCController.generated.h"


/** @brief 대화 유형 */
UENUM()
enum class ENPCConversationType :uint8 {
    P2N = 0,
    N2N,

};

/**
 * @author 전유진
 * @brief NPCController 클래스입니다.
 */
UCLASS()
class QUACKTOHELL_API AQNPCController : public AAIController
{
    GENERATED_BODY()
public:
    /**
     * @brief NPC와의 대화시작 요청을 처리하는 함수입니다. npc-npc, player-npc 대화시스템에 활용됩니다.
     * @param 대화거는 주체의 폰 정보를 넘깁니다.
     */
    void StartDialog(TObjectPtr<APawn> MyPawn, ENPCConversationType ConversationType);
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
    void Response(FString& Text);
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
    /** @brief OnNPCResponseReceived의 콜백함수 */
    UFUNCTION()
    void OnNPCResponseReceived(const FString& Text);
   
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
    /**
     * @brief. 얼음땡
     */
    void UnFreezePawn();
private:
    TObjectPtr<class AQNPC> MyPawn;
};





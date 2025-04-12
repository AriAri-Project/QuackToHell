// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "QNPC.h"
#include "Player/QPlayerState.h"
#include "QPlayer.generated.h"
class AQPlayerController;
/**
 * @author 전유진 유서현
 * @brief 플레이어 캐릭터 클래스입니다.
 */
UCLASS()
class QUACKTOHELL_API AQPlayer : public AQCharacter
{
	GENERATED_BODY()
	/*AQPlayerController,UQP2NWidget에게 정보은닉공개 허용*/
	friend class AQPlayerController;
	friend class UQP2NWidget;
public:
	/**
	 * @brief Player2N 스피치버블 위젯을 리턴합니다. NPCController에서 접근하기 위함입니다.
	 *
	 * @return Player2Nspeechbubblewidget
	 */
	TObjectPtr<class UQPlayer2NSpeechBubbleWidget> GetPlayer2NSpeechBubbleWidget() const;
public:
	AQPlayer(const FObjectInitializer& ObjectInitializer);
	/**
	 * @brief 캐릭터 기준으로 가장 가까이 있는 npc를 반환합니다.
	 * 
	 * @return 캐릭터 기준 가장 가까이 있는 npc
	 */
	TObjectPtr<AActor> GetClosestNPC();

protected:
	/**
	 * @brief Player2NSpeechBubble UI 컴포넌트입니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent>  Player2NSpeechBubbleWidgetComponent;
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	/**
	 * @brief 스프링암 컴포넌트입니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	/**
	 * @brief 카메라 컴포넌트입니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> CameraComponent;
	
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
	UPROPERTY()
	TObjectPtr<AQPlayerState> LocalPlayerState;
	
	// NPC 대화 관련 check 함수 ------------------------------------------------------
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/** @brief 해당 NPC와 대화가능한지 check*/
	UFUNCTION(Server, Reliable)
	void ServerRPCCanStartConversP2N(AQPlayerController* TargetController,  AQNPC* NPC);

	/** @brief 해당 NPC와의 대화를 마칠 수 있는 check*/
	UFUNCTION(Server, Reliable)
	void ServerRPCCanFinishConversP2N(AQPlayerController* TargetController,  AQNPC* NPC);


	// NPC 대화 관련 대화 실행/마무리 함수 ---------------------------------------------------
	/** @brief NPC와의 대화 시작.*/
	UFUNCTION(Server, Reliable)
	void ServerRPCStartConversation(AQNPC* NPC);
	
	/** @brief NPC와의 대화 마무리*/
	UFUNCTION(Server, Reliable)
	void ServerRPCFinishConversation(AQPlayerController* TargetController,  AQNPC* NPC);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCStartConversation(AQPlayer* Player, AQNPC* NPC);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCFinishConversation(AQPlayer* Player, AQNPC* NPC);
	
private:
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/** @brief overlap에 들어온 대상을 담습니다. */
	TArray<TObjectPtr<AActor>> OverlappingNPCs;
private:
	/** @brief 허공말풍선 위젯 클래스 정보를 담습니다. */
	UPROPERTY()
	TObjectPtr<class UQPlayer2NSpeechBubbleWidget> Player2NSpeechBubbleWidget;
};

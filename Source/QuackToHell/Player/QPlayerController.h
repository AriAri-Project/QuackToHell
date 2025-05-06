// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "QPlayerController.generated.h"
struct FInputActionValue;
/**
 * @author 전유진
 * @brief 플레이어 컨트롤러 클래스입니다.
 */
UCLASS()
class QUACKTOHELL_API AQPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	TObjectPtr<class AQVillageUIManager> GetVillageUIManager() const { return VillageUIManager; }
public:
	/**
	 * @brief 상호작용을 block합니다.
	 * 
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastBlockInteraction();
public:
	/** @breif ServerRPCStartConversation을 성공적으로 마치게 되면 실행되는 함수. 이 내부에 클라쪽 StartConversation 구현
	* @param NPC 대화대상 npc
	* @param NPCStartResponse NPC 시작 메세지
	*/
	UFUNCTION(Client, Reliable)
	void ClientRPCStartConversation(FOpenAIResponse NPCStartResponse);
	UFUNCTION(Client, Reliable)
	void ClientRPCFinishConversation(AQNPC* NPC);
	/** @breif ServerRPCCanFinishConversP2N를 통해 대화마무리가 가능한지 체크가 완료된 후 실행되는 클라이언트 RPC
	 * 인자로 마무리할 수 있는지 없는지에 대한 bool값이 들어오게 된다. */
	UFUNCTION(Client, Reliable)
	void ClientRPCUpdateCanFinishConversP2N(bool bResult);
	/** @breif ServerRPCCanStartConversP2N를 통해 대화시작이 가능한지 체크가 완료된 후 실행되는 클라이언트 RPC
	 * 인자로 시작할 수 있는지 없는지에 대한 bool값이 들어오게 된다. */
	UFUNCTION(Client, Reliable)
	void ClientRPCUpdateCanStartConversP2N(bool bResult);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
protected:
	/** @brief IMC입니다. */
	UPROPERTY(EditAnyWhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	/** @brief Interaction IA 입니다. */
	UPROPERTY(EditAnyWhere, Category = "Input")
	TObjectPtr<class UInputAction> InteractionAction;
	/** @brief Move IA 입니다. */
	UPROPERTY(EditAnyWhere, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;
	/** @brief Turn IA 입니다. */
	UPROPERTY(EditAnyWhere, Category = "Input")
	TObjectPtr<class UInputAction> TurnAction;
	/** @brief Enable Turn IA 입니다. true일 경우에만 turn이 활성화됩니다. */
	UPROPERTY(EditAnyWhere, Category = "Input")
	TObjectPtr<class UInputAction> EnableTurnAction;
	/** @brief TurnOnOff Map IA 입니다. true일 경우 지도가 활성화/비활성화 됩니다. */
	UPROPERTY(EditAnyWhere, Category = "Input")
	TObjectPtr<class UInputAction> TurnOnOffMapAction;
	/** @brief TurnOnOff Inventory IA 입니다. true일 경우 인벤토리가 활성화/비활성화 됩니다. */
	UPROPERTY(EditAnyWhere, Category = "Input")
	TObjectPtr<class UInputAction> TurnOnOffInventoryAction;
private:
	/**
	 * @brief 이동 입력 처리 함수입니다..
	 *
	 * @param InputValue 입력 값
	 */
	void InputMove(const FInputActionValue& InputValue);
	/**
	 * @brief 회전 입력 처리 함수입니다..
	 *
	 * @param InputValue 입력 값
	 */
	void InputTurn(const FInputActionValue& InputValue);
	/**
	 * @brief 회전 활성화 처리 함수입니다.
	 *
	 * @param InputValue 입력 값
	 */
	void InputEnableTurn(const FInputActionValue& InputValue);
	/**
	 * @brief 상호작용 입력 처리 함수입니다..
	 *
	 * @param InputValue 입력 값
	 */
	void InputInteraction(const FInputActionValue& InputValue);
	/**
	 * @brief 맵 Turn On/Off 입력 처리 함수입니다. InputValue가 true면 끄거나 킵니다.
	 *
	 * @param InputValue 입력 값
	 */
	void InputTurnOnOffMap(const FInputActionValue& InputValue);
	/**
	 * @brief 인벤토리 Turn On/Off 입력 처리 함수입니다. InputValue가 true면 끄거나 킵니다.
	 *
	 * @param InputValue 입력 값
	 */
	void InputTurnOnOffInventory(const FInputActionValue& InputValue);
	
	
	/**
	 * @brief 회전 활성화 처리 함수함수에서 true가 들어올 시, 이 변수도 true됩니다.
	 */
	bool bEnableTurn = false;
private:

	
	/**
	 * @brief 몸을 멈춥니다.
	 */
	void FreezePawn();
	
	/**
	 * @brief 몸을 얼음땡 합니다.
	 */
	void UnFreezePawn();


private:
	/** @brief VillageUIManager정보를 갖습니다. */
	TObjectPtr<class AQVillageUIManager> VillageUIManager;
private:
	/* 재판장이동 타이머 */
	float MoveToCourtTimer = 0.0f;
	const float MoveToCourtTimerMax = 3.0f;
};





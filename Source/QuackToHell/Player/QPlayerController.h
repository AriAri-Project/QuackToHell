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
	/**
	* @brief 대화끝날때의 처리를 구현합니다. 대화끝의 트리거를 외부에서 수집하므로, 외부에 public으로 해당 함수를 열었습니다. 1. 얼음땡 2. 상태전환
	* @param 대화중인 NPC
	*/
	void ConverseEndProcess(TObjectPtr<class AQNPC> NPC);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
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
	 * @brief 대화를 시작합니다.
	 */
	void StartDialog();
	/**
	 * @brief 대화를 중단합니다.
	 */
	void EndDialog();
	
	/**
	 * @brief 몸을 멈춥니다.
	 */
	void FreezePawn();
	
	/**
	 * @brief 몸을 얼음땡 합니다.
	 */
	void UnFreezePawn();
	/**
	 * @brief 대화중일때의 처리를 구현합니다. 1. 몸 멈추기 2. 대화상태로 전환하기
	 */
	void ConverseProcess();
private:
	/** @brief VillageUIManager정보를 갖습니다. */
	TObjectPtr<class AQVillageUIManager> VillageUIManager;

};





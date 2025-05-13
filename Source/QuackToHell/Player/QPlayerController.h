// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/QVillageUIManager.h"
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
	void BlockInteraction();

	/**
	 * @return 플레이어의 마을 UI 매니저 반환
	 */
	AQVillageUIManager* GetVillageUIManager()
	{
		return VillageUIManager;
	}
	
	/** @breif ServerRPCStartConversation을 성공적으로 마치게 되면 실행되는 함수. 이 내부에 클라쪽 StartConversation 구현
	* @param NPC 대화대상 npc
	* @param NPCStartResponse NPC 시작 메세지
	*/
	UFUNCTION(Client, Reliable)
	void ClientRPCStartConversation(FOpenAIResponse NPCStartResponse, AQNPC* NPC);

	UFUNCTION(Client, Reliable)
	void ClientRPCUpdateP2NResponse(FOpenAIResponse Response);
	
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

public:
	// -- 재판장 관련 RPC 함수 -- //
	/**
	 * @brief 서버에 매치메이킹 시작을 요청하는 RPC 함수
	 * @details [캡스톤] 클라이언트가 매치메이킹(세션 생성 또는 참가)을 요청할 때 호출되며, 서버에서 매치메이킹 로직을 실행함
	 * @param ClientPC 매치메이킹을 요청한 클라이언트의 PlayerController
	 */
	UFUNCTION(Server, Reliable)
	void ServerRPCStartMatchMaking(AQPlayerController* ClientPC);
	
	/**
	 * @brief 검사측 진술을 요청하는 서버 RPC 함수
	 * @details [캡스톤] 클라이언트가 검사측 진술을 요청하면, 서버에서 관련 AI 응답 또는 대사를 처리함
	 * @param ClientPC 요청을 보낸 클라이언트의 PlayerController
	 */
	UFUNCTION(Server, Reliable)
	void ServerRPCGetProsecutorStatement(AQPlayerController* ClientPC);

	/**
	 * @brief 클라이언트에게 검사측 진술을 전달하는 RPC 함수
	 * @details [캡스톤] 서버에서 생성된 검사측 진술 응답을 클라이언트에게 전달하여 UI에 반영함
	 * @param Response 검사측 진술에 대한 AI 응답 데이터
	 */
	UFUNCTION(Server, Reliable)
	void ClientRPCGetProsecutorStatement(FOpenAIResponse Response);

	/**
	 * @brief 변호인측 진술을 요청하는 서버 RPC 함수
	 * @details [캡스톤] 클라이언트가 변호인측 진술을 요청하면, 서버에서 관련 AI 응답 또는 대사를 처리함
	 * @param ClientPC 요청을 보낸 클라이언트의 PlayerController
	 */
	UFUNCTION(Server, Reliable)
	void ServerRPCGetLawyerStatement(AQPlayerController* ClientPC);

	/**
	 * @brief 클라이언트에게 변호인측 진술을 전달하는 RPC 함수
	 * @details [캡스톤] 서버에서 생성된 변호인측 진술 응답을 클라이언트에게 전달하여 UI에 반영함
	 * @param Response 변호인측 진술에 대한 AI 응답 데이터
	 */
	UFUNCTION(Server, Reliable)
	void ClientRPCGetLawyerStatement(FOpenAIResponse Response);

	/**
	 * @brief 최종 판결 요청을 서버에 전달하는 RPC 함수
	 * @details [캡스톤] 클라이언트가 재판 결과(최종 판단)를 요청하면, 서버가 AI 응답 또는 로직 처리 후 결과를 생성함
	 * @param ClientPC 요청을 보낸 클라이언트의 PlayerController
	 */
	UFUNCTION(Server, Reliable)
	void ServerRPCGetFinalJudgement(AQPlayerController* ClientPC);

	/**
	 * @brief 클라이언트에게 최종 판결을 전달하는 RPC 함수
	 * @details [캡스톤] 서버가 생성한 최종 판결 결과를 클라이언트에 전달하여 UI에 출력되도록 함
	 * @param Response 최종 판결에 대한 AI 응답 데이터
	 */
	UFUNCTION(Server, Reliable)
	void ClientRPCGetFinalJudgement(FOpenAIResponse Response);

	/**
	 * @brief 클라이언트를 메인 맵으로 이동시키는 서버 RPC 함수
	 * @details [캡스톤] 재판 종료 등 특정 조건에서 클라이언트를 메인 씬으로 Seamless Travel 방식으로 전환시킴
	 * @param ClientPC 메인 맵으로 이동시킬 대상 클라이언트의 PlayerController
	 */
	UFUNCTION(Server, Reliable)
	void ServerRPCTravelToMain(AQPlayerController* ClientPC);


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





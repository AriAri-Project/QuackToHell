// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QP2NWidget.generated.h"


/**
 * @author 전유진
 * @brief P2N 대화 UI입니다.
 */
UCLASS()
class QUACKTOHELL_API UQP2NWidget : public UUserWidget
{
	GENERATED_BODY()
public:


	/**
	 * @brief. NPC에게 답장올 시 처리해야되는 로직이 실행된다.
	 */
	UFUNCTION()
	void DisplayNPCResponse(FOpenAIResponse NPCStartResponse);
public:
	/**
	 * @brief 대화상자의 글씨를 업데이트합니다.
	 * @param 새로 띄울 텍스트를 인자로 넣습니다.
	 */
	void UpdatePlayerText(const FString& Text);
	/**
	 * @brief 대화상자의 글씨를 업데이트합니다.
	 * @param 새로 띄울 텍스트를 인자로 넣습니다.
	 */
	UFUNCTION() //델리게이트 콜백
	void UpdateNPCText(const FString& Text);
	
	/**
	 * @brief 대화중인 NPC정보를 세팅합니다..
	 * 
	 * @param NPC
	 */
	void SetConversingNPC(const TObjectPtr<class AQDynamicNPC> NPC);
	/**
	 * @brief 대화중인 player정보를 세팅합니다..
	 *
	 * @param player
	 */
	void SetConversingPlayer(const TObjectPtr<class AQPlayerController> Player);
	/** @brief 엔터키 눌렸을때의 처리; */
	UFUNCTION(BlueprintCallable)
	void HandleEnterKeyPress();
	/** @brief 종료버튼 눌렸을때의 처리; */
	UFUNCTION(BlueprintCallable)
	void HandleEnterEndButton();


private:
	/**
	 * @brie PlayerText 컴포넌트 바인드
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerText;
	/**
	 * @brie NPCText 컴포넌트 바인드
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> NPCText;

	/**
	 * @brie inputBox 컴포넌트 바인드
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> inputBox;
	/** @brief 대화중인 npc정보 */
	UPROPERTY()
	TObjectPtr<class AQDynamicNPC> ConversingNPC;
	/** @brief 대화중인 플레이어정보 */
	UPROPERTY()
	TObjectPtr<class AQPlayerController> ConversingPlayer;

	/** @brief 응답생성중일때의 멘트 */
	const FString WhenGenerateResponseText = TEXT("음...");
};





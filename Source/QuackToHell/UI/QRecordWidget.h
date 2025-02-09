// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/QVillageGameState.h"
#include "QChatBoxWidget.h"
#include "QRecordWidget.generated.h"


/**
 * @brief 버튼의 정보를 가지는 구조체입니다.
 */
USTRUCT()
struct FMyButton {
	GENERATED_BODY()
private:
	UPROPERTY()
	TObjectPtr<class UButton> Button;

	UPROPERTY()
	TObjectPtr<class UOverlay> Overlay;

	UPROPERTY()
	TObjectPtr<class UTextBlock> GhostName;

	//해당 클래스에서만 접근가능하게.
	friend class UQRecordWidget;
};

USTRUCT()
struct FChat {
	GENERATED_BODY()
private:
	int32 ID;
	FString Message;
	friend class UQRecordWidget;
};

/**
 * @author 전유진
 * @brief 녹음기록 화면의 위젯입니다.
 */
UCLASS()
class QUACKTOHELL_API UQRecordWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UQRecordWidget(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void NativeConstruct() override;
private:
	/**
	 * @brief 해당 위젯의 하위컴포넌트인 버튼들의 정보를 갖고있음.
	 */
	UPROPERTY()
	TArray<FMyButton > Buttons;
private:
	/**
	 * @brief 최대 망자 수.
	 */
	const int8 GhostMaxNum = 8;
	/**
	 * @brief 대화기록 데이터들
	 */
	TArray<FConversationRecord> ConversationRecord;
	/**
	 * @brief. 대화한 NPC목록들
	 */
	TArray<int32> ConversedNPC;
private:
	/**
	 * @brief 대화기록을 업데이트합니다.
	 * 업데이트시점: 녹음기버튼이 눌렸을 때
	 * 
	 */
	UFUNCTION()
	void UpdateRecordHistory();
	/**
	 * @brief 만약 버튼이 눌리면, 채팅내역 위젯을 띄웁니다.
	 * @param 어떤 NPC 의 대화기록을 요구하는건지, NPCID를 인자로 넣습니다.
	 */
	UFUNCTION(BlueprintCallable)
	void TurnOnChatWidget(FText NPCID);
	
	/**
	 * @brief 버튼위젯의 모양을 업데이트한다.
	 */
	void UpdateButtonOverlay();

private:
	/**
	 * @brie Vertical Box 컴포넌트 바인드
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> VerticalBox;
private:
	TArray<TArray<FChat>> ChatHistories;
private:
	/**
	 * @brief 챗박스 WBP 클래스정보.
	 */
	TSubclassOf<class UQChatBoxWidget> ClassOfChatBox;
	
};



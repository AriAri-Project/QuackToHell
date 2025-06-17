// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "QVillageUIManager.generated.h"
UENUM()
/** @brief 마을 내 존재해야하는 UI목록들 */
enum class EVillageUIType :uint8 {
	P2N= 0,
	DefaultVillageUI,
	Map,
	Player2NSpeechBubble,
	SpeechBubble,
	Inventory, 
	Record,
	VillageTimer,
	Evidence, // 증거물 인벤토리

};
UCLASS()
/**
 * @author 전유진
 * @brief 마을 맵 내에서만 존재하는 마을 UI manager. 싱글톤 객체로 관리됩니다.
 */
class QUACKTOHELL_API AQVillageUIManager : public AActor
{
	GENERATED_BODY()

public:
	/**
	 * @brief UI를 켭니다.
	 * 
	 * @param UIType
	 */
	void TurnOnUI(EVillageUIType UIType);
	/**
	 * @brief UI를 끕니다.
	 *
	 * @param UIType
	 */
	void TurnOffUI(EVillageUIType UIType);
	/**
	 * @brief 싱글톤 인스턴스 반환.
	 * @return 싱글톤 인스턴스
	 */
	static TObjectPtr<AQVillageUIManager> GetInstance(TObjectPtr<UWorld> World);
	/**
	 * @brief 마을의 위젯들을 리턴합니다.
	 * 
	 * @param 리턴받을 위젯의 key를 대입합니다.
	 * @return 위젯을 반드시 반환합니다.  
	 */
	UUserWidget* GetActivedWidget(EVillageUIType UIType) ;
	/**
	 * .@brief 타이머 종료 시 호출됩니다. 마을 UI를 마무리합니다.
	 *  @details 내부에서 1. 열린 UI 팝업 닫기 2. UI 상호작용 차단을 구현합니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEndupUI();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Called when level is unloded
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** @brief 싱글톤 인스턴스 */
	static TObjectPtr<AQVillageUIManager> Instance;
	/**
	 * @brief private생성자로 외부에서 객체생성을 막음.
	 * 
	 */
	AQVillageUIManager();

	/**
	 * @brief 마을 내 존재해야하는 위젯클래스들: UI타입과 위젯클래스 매핑.
	 * UIWidgetClasses에 따라서 UI를 생성한다: 해당 UI를 키라는 입력이 들어올 때
	 * 예외: 기본적으로 화면에 있어야 하는 건 초기화 때 생성& visible처리
	 */
	TMap<EVillageUIType, TSubclassOf<UUserWidget>> UIWidgetClasses;
	/** @brief 마을 위젯 관리 */
	TMap<EVillageUIType, TObjectPtr<UUserWidget>> ActivedVillageWidgets;
	
	/**
	 * @brief
	 * 맵 로드 시 실행.
	 * 
	 */
	void OnMapLoad();
	/**
	 * @brief.마을맵인지 체크 (마을맵이 아닐 시 싱글톤생성 x)
	 */
	bool IsVillageMap();
	/**
	 * @brief UI상호작용을 닫습니다. 즉, 상호작용이 안되도록 블락처리합니다.
	 */
	void CloseUIInteraction();
private:
	/* 헬퍼 메서드들 */
	/**
	 * @brief 내부 헬퍼: 위젯이 없으면 생성하고, 있으면 nullptr 반환하는 함수.
	 * 
	 * @param 검사할 UIType
	 * @return 위젯이 없으면 UserWidget반환, 위젯이 있으면 nullptr반환 
	 */
	UUserWidget* CreateWidgetIfNotExists(EVillageUIType UIType);
};





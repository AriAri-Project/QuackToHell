// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Character/QNPC.h"
#include "QDynamicNPC.generated.h"

/**
 * @author 전유진
 * @brief 스스로 움직이고, 역동적으로 상호작용(n2n, p2n)하는 NPC 캐릭터들을 담습니다.  
 */
UCLASS()
class QUACKTOHELL_API AQDynamicNPC : public AQNPC
{
	GENERATED_BODY()
public:
	AQDynamicNPC(const FObjectInitializer& ObjectInitializer);

public:
	/**
	 * @brief Player2N 스피치버블 위젯을 리턴합니다. NPCController에서 접근하기 위함입니다.
	 *
	 * @return Player2Nspeechbubblewidget
	 */
	TObjectPtr<class UQPlayer2NSpeechBubbleWidget> GetPlayer2NSpeechBubbleWidget() const;
public:
	/**
	 * @brief 캐릭터 기준으로 가장 가까이 있는 npc를 반환합니다.
	 *
	 * @return 캐릭터 기준 가장 가까이 있는 npc
	 */
	TObjectPtr<AActor> GetClosestNPC();
protected:

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	virtual void BeginPlay() override;
protected:
	/**
	 * @brief E UI 컴포넌트입니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent> EKeyWidgetComponent;
	/**
	 * @brief Player2NSpeechBubble UI 컴포넌트입니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent>  Player2NSpeechBubbleWidgetComponent;
private:
	/** @brief EKey위젯 클래스 정보를 담습니다. */
	UPROPERTY()
	TObjectPtr<class UQEKeyWidget> EKeyWidget;

	/** @brief 허공말풍선 위젯 클래스 정보를 담습니다. */
	UPROPERTY()
	TObjectPtr<class UQPlayer2NSpeechBubbleWidget> Player2NSpeechBubbleWidget;
private:

	/** @brief overlap에 들어온 대상을 담습니다. */
	TArray<TObjectPtr<class AQDynamicNPC>> OverlappingNPCs;
private:
	/**
	 * @brief EKey위젯 UI를 켭니다.
	 */
	void TurnOnEKeyUI();
	/**
	 * @brief EKey위젯 UI를 끕니다.
	 */
	void TurnOffEKeyUI();


};





// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Character/QNPC.h"
#include "GameData/QConversationData.h"
#include "GameData/QEvidenceData.h"
#include "GameFramework/GameState.h"
#include "QVillageGameState.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQVillageGameState : public AGameState
{
	GENERATED_BODY()

public:
	AQVillageGameState();

	virtual void Tick(float DeltaSeconds) override;
public:
	/**
	 * @brief 타이머가 울린 뒤, 마을활동을 마무리하는 함수입니다.
	 * 
	 */
	UFUNCTION(NetMulticast, Reliable)
	void EndVillageActivity();

private:
	/**
	 * @todo 60*7로 되돌리기.
	 */
	float TimeUntilTrialMax = 60*7;
	
	/** @brief 재판까지 남은 시간 */
	UPROPERTY(Replicated)
	float ServerLeftTimeUntilTrial = 0.0f;
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCUpdateServerTime();

private:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:

	/* 재판장 이동 */
	UFUNCTION(Server, Reliable)
	void ServerRPCRequestTravelToCourt(APlayerController* LocalPlayerController, bool bTravelToCourt);


	// 테스트 용
	virtual void BeginPlay() override;
	
	TSubclassOf<UUserWidget> StartLevelWidget;
};

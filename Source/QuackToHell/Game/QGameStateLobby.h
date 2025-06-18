// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "QGameStateLobby.generated.h"

class UQLobbyLevelWidget;
/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQGameStateLobby : public AGameState
{
	GENERATED_BODY()

	AQGameStateLobby();
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	

	UPROPERTY(ReplicatedUsing = OnRep_bIsClientReady)
	bool bIsClientReady = false;

	UPROPERTY(ReplicatedUsing = OnRep_bIsGameStarted)
	bool bIsGameStarted = false;

	UFUNCTION()
	void OnRep_bIsClientReady();

	UFUNCTION()
	void OnRep_bIsGameStarted();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCChangeClientReadyState();
	
	UFUNCTION()
	void HostGameStart();
};

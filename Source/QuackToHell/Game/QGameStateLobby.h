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
	UPROPERTY(Replicated)
	bool bIsClientReady = false;

	UPROPERTY(Replicated)
	FString HostName = "";

	UPROPERTY(Replicated)
	FString ClientName = "";

	UFUNCTION()
	void UpdateIsClientReady();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPCAlertNewPlayer(const FString& NewPlayerName);
};

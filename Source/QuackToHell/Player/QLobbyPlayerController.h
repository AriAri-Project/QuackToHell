// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "QLobbyPlayerController.generated.h"

class UQLobbyLevelWidget;
/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

	AQLobbyPlayerController();

public:
	TSubclassOf<UQLobbyLevelWidget> LobbyLevelWidget;
	TObjectPtr<UQLobbyLevelWidget> CachedLobbyWidget;
	
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowLobbyUI();

	UFUNCTION(Client, Reliable)
	void ClientRPC_NewPlayerAlert(const FString& PlayerName);
};

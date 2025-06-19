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
	virtual void BeginPlay() override;

public:
	TSubclassOf<UQLobbyLevelWidget> LobbyLevelWidget;
	TObjectPtr<UQLobbyLevelWidget> CachedLobbyWidget;
	
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowLobbyUI();

	UFUNCTION()
	void NewPlayerAlert(const FString& PlayerName);

	UFUNCTION()
	void UpdateHostName(const FString& HostName);

	UFUNCTION()
	void UpdateClientName(const FString& ClientName);
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_ToggleClientReady();
	
};



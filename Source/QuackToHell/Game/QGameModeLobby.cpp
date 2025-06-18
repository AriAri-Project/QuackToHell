// Copyright_Team_AriAri


#include "Game/QGameModeLobby.h"

#include "QGameStateLobby.h"
#include "Blueprint/UserWidget.h"
#include "Player/QLobbyPlayerController.h"
#include "Player/QPlayerState.h"

AQGameModeLobby::AQGameModeLobby()
{
	bReplicates = true;
	
	PlayerControllerClass = AQLobbyPlayerController::StaticClass();
	PlayerStateClass = AQPlayerState::StaticClass();
	GameStateClass = AQGameStateLobby::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(TEXT("/Game/Blueprints/Character/BP_StartPlayer.BP_StartPlayer_C"));
	if (PawnClassRef.Class)
	{
		DefaultPawnClass = PawnClassRef.Class;
	}
}

void AQGameModeLobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AQPlayerState* PlayerState = Cast<AQPlayerState>(NewPlayer->PlayerState);
	if (!PlayerState)
	{
		return;
	}
	
	TObjectPtr<AQLobbyPlayerController> LobbyPC = Cast<AQLobbyPlayerController>(NewPlayer);
	LobbyPC->ClientRPC_ShowLobbyUI();
	LobbyPC->ClientRPC_NewPlayerAlert(PlayerState->GetPlayerName());
}

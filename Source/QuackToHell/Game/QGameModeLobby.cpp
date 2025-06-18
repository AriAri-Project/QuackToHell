// Copyright_Team_AriAri


#include "Game/QGameModeLobby.h"

#include "QGameStateLobby.h"
#include "Blueprint/UserWidget.h"
#include "Player/QPlayerState.h"
#include "Player/QStartPlayerController.h"

AQGameModeLobby::AQGameModeLobby()
{
	PlayerControllerClass = AQStartPlayerController::StaticClass();
	PlayerStateClass = AQPlayerState::StaticClass();
	GameStateClass = AQGameStateLobby::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(TEXT("/Game/Blueprints/Character/BP_StartPlayer.BP_StartPlayer_C"));
	if (PawnClassRef.Class)
	{
		DefaultPawnClass = PawnClassRef.Class;
	}
}

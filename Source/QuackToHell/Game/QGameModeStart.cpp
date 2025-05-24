// Copyright_Team_AriAri


#include "Game/QGameModeStart.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "QStartPlayerController.h"
#include "Engine/Engine.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "UObject/ConstructorHelpers.h"

AQGameModeStart::AQGameModeStart()
{
	PlayerControllerClass = AQStartPlayerController::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(TEXT("/Game/Blueprints/Character/BP_StartPlayer.BP_StartPlayer_C"));
	if (PawnClassRef.Class)
	{
		DefaultPawnClass = PawnClassRef.Class;
	}
}

void AQGameModeStart::BeginPlay()
{
	Super::BeginPlay();
}


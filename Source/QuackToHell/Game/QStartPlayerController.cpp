// Copyright_Team_AriAri


#include "Game/QStartPlayerController.h"

AQStartPlayerController::AQStartPlayerController()
{
	bShowMouseCursor = true;
	
}

void AQStartPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

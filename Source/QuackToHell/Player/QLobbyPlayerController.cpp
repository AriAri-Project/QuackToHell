// Copyright_Team_AriAri


#include "Player/QLobbyPlayerController.h"
#include "UI/Lobby/QLobbyLevelWidget.h"

AQLobbyPlayerController::AQLobbyPlayerController()
{
	static ConstructorHelpers::FClassFinder<UQLobbyLevelWidget> LobbyWidgetAsset(TEXT("/Game/Blueprints/UI/Lobby/WBP_LobbyLevel"));
	if (LobbyWidgetAsset.Succeeded())
	{
		LobbyLevelWidget = LobbyWidgetAsset.Class;
	}
}

void AQLobbyPlayerController::ClientRPC_ShowLobbyUI_Implementation()
{
	if (LobbyLevelWidget)
	{
		CachedLobbyWidget = CreateWidget<UQLobbyLevelWidget>(GetWorld(), LobbyLevelWidget);
		if (CachedLobbyWidget)
		{
			CachedLobbyWidget->AddToViewport();
		}
	}
}

void AQLobbyPlayerController::ClientRPC_NewPlayerAlert_Implementation(const FString& PlayerName)
{
	if (CachedLobbyWidget && CachedLobbyWidget->IsInViewport())
	{
		const FString& Message = FString::Printf(TEXT("%s님이 입장하셨습니다."), *PlayerName);
		CachedLobbyWidget->AppendLogMessage(Message);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Lobby] CachedLobbyWidget가 아직 초기화되지 않았습니다."));
	}
}



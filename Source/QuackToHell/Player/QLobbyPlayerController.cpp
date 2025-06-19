// Copyright_Team_AriAri


#include "Player/QLobbyPlayerController.h"

#include "QLogCategories.h"
#include "Components/TextBlock.h"
#include "Game/QGameStateLobby.h"
#include "UI/Lobby/QLobbyLevelWidget.h"

AQLobbyPlayerController::AQLobbyPlayerController()
{
	static ConstructorHelpers::FClassFinder<UQLobbyLevelWidget> LobbyWidgetAsset(TEXT("/Game/Blueprints/UI/Lobby/WBP_LobbyLevel"));
	if (LobbyWidgetAsset.Succeeded())
	{
		LobbyLevelWidget = LobbyWidgetAsset.Class;
	}
}

void AQLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AQLobbyPlayerController::ClientRPC_ShowLobbyUI_Implementation()
{
	if (!LobbyLevelWidget)
	{
		return;
	}
	CachedLobbyWidget = CreateWidget<UQLobbyLevelWidget>(GetWorld(), LobbyLevelWidget);
	if (!CachedLobbyWidget)
	{
		return;
	}
	CachedLobbyWidget->AddToViewport();
}

void AQLobbyPlayerController::NewPlayerAlert(const FString& PlayerName)
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

void AQLobbyPlayerController::UpdateHostName(const FString& HostName)
{
	CachedLobbyWidget->SetHostNames(HostName);
}

void AQLobbyPlayerController::UpdateClientName(const FString& ClientName)
{
	CachedLobbyWidget->SetClientNames(ClientName);
}

void AQLobbyPlayerController::ServerRPC_ToggleClientReady_Implementation()
{
	AQGameStateLobby* LobbyGS = GetWorld() ? GetWorld()->GetGameState<AQGameStateLobby>() : nullptr;
	if (LobbyGS)
	{
		LobbyGS->UpdateIsClientReady();
	}
}
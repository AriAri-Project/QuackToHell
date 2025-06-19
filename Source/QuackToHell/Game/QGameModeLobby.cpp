// Copyright_Team_AriAri


#include "Game/QGameModeLobby.h"

#include "QGameStateLobby.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Player/QLobbyPlayerController.h"
#include "Player/QPlayerState.h"
#include "UI/Lobby/QLobbyLevelWidget.h"

AQGameModeLobby::AQGameModeLobby()
{
	bReplicates = true;
	bUseSeamlessTravel = true;
	
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
	UE_LOG(LogTemp, Warning, TEXT("[SERVER] PostLogin called. Player: %s"), *NewPlayer->GetName());
	
	AQPlayerState* PlayerState = Cast<AQPlayerState>(NewPlayer->PlayerState);
	AQGameStateLobby* LobbyGS = GetGameState<AQGameStateLobby>();
	if (!LobbyGS || !PlayerState)
	{
		return;
	}
	FString NewPlayerName = PlayerState->GetPlayerName();
	
	TObjectPtr<AQLobbyPlayerController> LobbyPC = Cast<AQLobbyPlayerController>(NewPlayer);
	if (LobbyPC->HasAuthority())
	{
		LobbyGS->HostName = NewPlayerName;
	}
	else
	{
		LobbyGS->ClientName = NewPlayerName;
		AQLobbyPlayerController* LocalPC = Cast<AQLobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
		LocalPC->UpdateClientName(NewPlayerName);
	}
	
	LobbyPC->ClientRPC_ShowLobbyUI();
	LobbyGS->MultiCastRPCAlertNewPlayer(NewPlayerName);
}

void AQGameModeLobby::HostGameStart()
{
	UE_LOG(LogTemp, Warning, TEXT("[SERVER] HostGameStart called. Seamless travel to Village map."));

	FString VillageMapPath = TEXT("/Game/Maps/VillageMap?Listen"); 
	GetWorld()->ServerTravel(VillageMapPath, true); // bSeamless = true
}
// Copyright_Team_AriAri


#include "Game/QGameStateLobby.h"

#include "QLogCategories.h"
#include "UI/Lobby/QLobbyLevelWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/QLobbyPlayerController.h"

AQGameStateLobby::AQGameStateLobby()
{
	
}

void AQGameStateLobby::BeginPlay()
{
	Super::BeginPlay();

	AQLobbyPlayerController* LocalPC = Cast<AQLobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
	LocalPC->CachedLobbyWidget->ClientName->SetText(FText::FromString(ClientName));
	LocalPC->CachedLobbyWidget->HostName->SetText(FText::FromString(HostName));
}

void AQGameStateLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQGameStateLobby, bIsClientReady);
	DOREPLIFETIME(AQGameStateLobby, HostName);
	DOREPLIFETIME(AQGameStateLobby, ClientName);
}

void AQGameStateLobby::UpdateIsClientReady()
{
	bIsClientReady = !bIsClientReady;
	AQLobbyPlayerController* LocalPC = Cast<AQLobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (LocalPC && LocalPC->HasAuthority() && GetNetMode() == NM_ListenServer)
	{
		// 클라이언트가 준비되었다면 게임시작 버튼 활성화
		LocalPC->CachedLobbyWidget->UpdateHostButton(bIsClientReady);
	}
}

void AQGameStateLobby::MultiCastRPCAlertNewPlayer_Implementation(const FString& NewPlayerName)
{
	TObjectPtr<AQLobbyPlayerController> LobbyPC = Cast<AQLobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
	LobbyPC->NewPlayerAlert(NewPlayerName);
}
// Copyright_Team_AriAri


#include "Game/QGameStateLobby.h"
#include "UI/Lobby/QLobbyLevelWidget.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"

AQGameStateLobby::AQGameStateLobby()
{
	
}

void AQGameStateLobby::BeginPlay()
{
	Super::BeginPlay();

	//테스트용 위젯 띄우기
	
}

void AQGameStateLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQGameStateLobby, bIsClientReady);
	DOREPLIFETIME(AQGameStateLobby, bIsGameStarted);
}

void AQGameStateLobby::OnRep_bIsClientReady()
{
}

void AQGameStateLobby::OnRep_bIsGameStarted()
{
}

void AQGameStateLobby::ServerRPCChangeClientReadyState_Implementation()
{
}

void AQGameStateLobby::HostGameStart()
{
	if (!HasAuthority())
	{
		return;
	}

	
}
// Copyright_Team_AriAri


#include "Game/QGameStateLobby.h"
#include "UI/Lobby/QLobbyLevelWidget.h"
#include "Blueprint/UserWidget.h"

AQGameStateLobby::AQGameStateLobby()
{
	static ConstructorHelpers::FClassFinder<UQLobbyLevelWidget> LobbyWidgetAsset(TEXT("/Game/Blueprints/UI/Lobby/WBP_LobbyLevel"));
	if (LobbyWidgetAsset.Succeeded())
	{
		LobbyLevelWidget = LobbyWidgetAsset.Class;
	}
}

void AQGameStateLobby::BeginPlay()
{
	Super::BeginPlay();

	//테스트용 위젯 띄우기
	if (LobbyLevelWidget)
	{
		UQLobbyLevelWidget* StartWidget = CreateWidget<UQLobbyLevelWidget>(GetWorld()->GetFirstPlayerController(), LobbyLevelWidget);
		if (StartWidget)
		{
			// 위젯을 화면에 추가
			StartWidget->AddToViewport();
		}
	}
}

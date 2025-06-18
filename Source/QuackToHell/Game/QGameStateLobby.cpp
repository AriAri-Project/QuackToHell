// Copyright_Team_AriAri


#include "Game/QGameStateLobby.h"
#include "UI/Lobby/QLobbyLevelWidget.h"
#include "Blueprint/UserWidget.h"

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

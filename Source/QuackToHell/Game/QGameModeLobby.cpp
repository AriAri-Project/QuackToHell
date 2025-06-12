// Copyright_Team_AriAri


#include "Game/QGameModeLobby.h"
#include "Blueprint/UserWidget.h"
#include "Player/QPlayerState.h"
#include "Player/QStartPlayerController.h"

AQGameModeLobby::AQGameModeLobby()
{
	PlayerControllerClass = AQStartPlayerController::StaticClass();
	PlayerStateClass = AQPlayerState::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(TEXT("/Game/Blueprints/Character/BP_StartPlayer.BP_StartPlayer_C"));
	if (PawnClassRef.Class)
	{
		DefaultPawnClass = PawnClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("/Game/Blueprints/UI/Lobby/WBP_LobbyLevel"));
	if (WidgetFinder.Succeeded())
	{
		LobbyLevelWidget = WidgetFinder.Class;
	}
}

void AQGameModeLobby::BeginPlay()
{
	Super::BeginPlay();

	Super::BeginPlay();

	//테스트용 위젯 띄우기
	if (LobbyLevelWidget)
	{
		UUserWidget* StartWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), LobbyLevelWidget);
		if (StartWidget)
		{
			// 위젯을 화면에 추가
			StartWidget->AddToViewport();
		}
	}
}

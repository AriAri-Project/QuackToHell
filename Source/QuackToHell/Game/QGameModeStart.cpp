// Copyright_Team_AriAri


#include "Game/QGameModeStart.h"
#include "Player/QStartPlayerController.h"
#include "Engine/Engine.h"
#include "Player/QPlayerState.h"
#include "UObject/ConstructorHelpers.h"

AQGameModeStart::AQGameModeStart()
{
	PlayerControllerClass = AQStartPlayerController::StaticClass();
	PlayerStateClass = AQPlayerState::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(TEXT("/Game/Blueprints/Character/BP_StartPlayer.BP_StartPlayer_C"));
	if (PawnClassRef.Class)
	{
		DefaultPawnClass = PawnClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("/Game/Blueprints/UI/Lobby/WBP_StartLevel"));
	if (WidgetFinder.Succeeded())
	{
		StartLevelWidget = WidgetFinder.Class;
	}
}

void AQGameModeStart::BeginPlay()
{
	Super::BeginPlay();

	//테스트용 위젯 띄우기
	if (StartLevelWidget)
	{
		UUserWidget* StartWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), StartLevelWidget);
		if (StartWidget)
		{
			// 위젯을 화면에 추가
			StartWidget->AddToViewport();
		}
	}
}


// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "QGameStateLobby.generated.h"

class UQLobbyLevelWidget;
/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQGameStateLobby : public AGameState
{
	GENERATED_BODY()
	AQGameStateLobby();
	virtual void BeginPlay() override;
	
public:
	TSubclassOf<UQLobbyLevelWidget> LobbyLevelWidget;
};

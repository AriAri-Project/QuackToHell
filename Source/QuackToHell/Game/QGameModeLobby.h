// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "QGameModeLobby.generated.h"

class UQLobbyWidget;
class UTextBlock;
/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQGameModeLobby : public AGameMode
{
	GENERATED_BODY()

	AQGameModeLobby();

public:
	UFUNCTION(BlueprintCallable)
	
	
private:
	virtual void BeginPlay() override;
	TSubclassOf<UQLobbyWidget> LobbyLevelWidget;
};

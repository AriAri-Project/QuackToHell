// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "QGameStateCourt.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQGameStateCourt : public AGameState
{
	GENERATED_BODY()

public:
	/** @brief 모두 진술 입력창으로 전환할 수 있음을 서버에게 알리는 ServerRPC 함수 */
	UFUNCTION(Server, Reliable)
	void ServerRPCConverseToEnterStatement(APlayerController* LocalPlayerController, bool bEnterStatement);
	
};

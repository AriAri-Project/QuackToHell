// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ClientTestGameMode.generated.h"

/**
 * @author 전유진
 * @brief 클라이언트 구현 테스트를 위한 게임모드입니다.
 */
UCLASS()
class QUACKTOHELL_API AClientTestGameMode : public AGameMode
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay();
};

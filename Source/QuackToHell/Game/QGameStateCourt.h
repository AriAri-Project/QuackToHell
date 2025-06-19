// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/QCourtUIManager.h"
#include "QGameInstance.h"
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
	TArray<FStatementEntry>& GetOpeningStatements() ;
private:
	/**
	 * @brief 서버, 클라 모두의 모두진술 대사를 보관함.
	 */
	UPROPERTY(Replicated)
	TArray<FStatementEntry> OpeningStatements;
public:
	AQGameStateCourt();
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	/**
	 * @brief 서버가 Opening 연출 시작을 지시하는 RPC
	 * @param DirectionType 어떤 연출을 실행할지(Opening 등)
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPCStartDirection(ECourtDirectionType DirectionType);
	
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPCStartUI(ECourtUIType UIType);
//public:
//	/** @brief 모두 진술 입력창으로 전환할 수 있음을 서버에게 알리는 ServerRPC 함수 */
//	UFUNCTION(NetMulticast, Reliable)
//	void ServerRPCConverseToEnterStatement(APlayerController* LocalPlayerController, bool bEnterStatement);


};

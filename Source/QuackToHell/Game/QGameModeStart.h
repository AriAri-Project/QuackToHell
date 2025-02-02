// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h" // 필수 헤더
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "QGameModeStart.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQGameModeStart : public AGameModeBase
{
	GENERATED_BODY()
public:
	AQGameModeStart();
	virtual void BeginPlay() override;

private:
	TSubclassOf<UUserWidget> StartLevelWidget;
	FString NewSessionName = "new session name yeah!";
	
	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	// delegate 선언
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	
	UFUNCTION(BlueprintCallable)
	void CreateSession();
	// Session이 Create되면 콜백될 함수
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void FindSession();
	// Session이 Find되면 콜백될 함수
	void OnFindSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void JoinSession();
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};

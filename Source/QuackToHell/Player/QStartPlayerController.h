// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "QStartPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API AQStartPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AQStartPlayerController();

private:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	FString NewSessionName = "Default Session Name";
	FString PlayerNickName = "Default Player";
	
	IOnlineSessionPtr OnlineSessionInterface;
	IOnlineIdentityPtr OnlineIdentity;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	// delegate 선언
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	UFUNCTION(BlueprintCallable)
	void SetSessionName(FString SessionName);
	
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

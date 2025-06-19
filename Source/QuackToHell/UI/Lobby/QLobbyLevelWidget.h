// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "QLobbyLevelWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class QUACKTOHELL_API UQLobbyLevelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	// TextBlock들 바인딩. 반드시 UMG에서 IsVariable 체크되어 있어야 함
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HostName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ClientName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LogText;

	UPROPERTY(meta = (BindWidget))
	UButton* GameButton;

	bool bIsReady = false;
	
	UFUNCTION(BlueprintCallable)
	void SetHostNames(const FString& Host);
	
	UFUNCTION(BlueprintCallable)
	void SetClientNames(const FString& Client);

	void AppendLogMessage(const FString& Message);

	void ChangeButtonImage(UTexture2D* NewTexture);

	UFUNCTION(BlueprintCallable)
	void OnHostGameButtonClicked();
	
	UFUNCTION(BlueprintCallable)
	void OnClientGameButtonClicked();

	UFUNCTION(BlueprintCallable)
	void UpdateHostButton(bool IsEnabled);

	UFUNCTION(BlueprintCallable)
	void UpdateClientButton(bool IsEnabled);
};
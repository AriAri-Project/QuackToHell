// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	UFUNCTION(BlueprintCallable)
	void SetHostNames(const FString& Host);
	
	UFUNCTION(BlueprintCallable)
	void SetClientNames(const FString& Client);

	// TextBlock들 바인딩. 반드시 UMG에서 IsVariable 체크되어 있어야 함
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HostName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ClientName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LogText;

	void AppendLogMessage(const FString& Message);
};

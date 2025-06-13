// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLobbyWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class QUACKTOHELL_API UQLobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UdateTextBlock(UTextBlock* TextBlock, FString NewText);
	void UdateClientNameTB();
	void UDateHostNameTB();

private:
	UQLobbyWidget();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HostNameTB;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ClientNameTB;
};

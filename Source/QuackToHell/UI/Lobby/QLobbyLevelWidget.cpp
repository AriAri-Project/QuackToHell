// Copyright_Team_AriAri


#include "UI/Lobby/QLobbyLevelWidget.h"
#include "Components/TextBlock.h"

void UQLobbyLevelWidget::SetHostNames(const FString& Host)
{
	if (HostName)
	{
		HostName->SetText(FText::FromString(Host));
	}
}

void UQLobbyLevelWidget::SetClientNames(const FString& Client)
{
	if (ClientName)
	{
		ClientName->SetText(FText::FromString(Client));
	}
}

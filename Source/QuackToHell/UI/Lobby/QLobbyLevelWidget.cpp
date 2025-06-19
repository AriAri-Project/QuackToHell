// Copyright_Team_AriAri


#include "UI/Lobby/QLobbyLevelWidget.h"
#include "Components/TextBlock.h"
#include "Game/QGameModeLobby.h"
#include "Game/QGameStateLobby.h"
#include "Kismet/GameplayStatics.h"
#include "Player/QLobbyPlayerController.h"


void UQLobbyLevelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AQLobbyPlayerController* LocalPC = Cast<AQLobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
	if (!LocalPC)
	{
		return;
	}

	GameButton->OnClicked.Clear();
	if (LocalPC->HasAuthority())
	{
		// 호스트 버튼 초기화
		UTexture2D* LoadedTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Art/UI/GameStart.GameStart"));
		ChangeButtonImage(LoadedTexture);
		GameButton->SetIsEnabled(false);
		GameButton->OnClicked.AddDynamic(this, &UQLobbyLevelWidget::OnHostGameButtonClicked);
	}
	else
	{
		// 클라이언트 버튼 초기화
		UTexture2D* LoadedTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Art/UI/Ready.Ready"));
		ChangeButtonImage(LoadedTexture);
		GameButton->SetIsEnabled(true);
		GameButton->OnClicked.AddDynamic(this, &UQLobbyLevelWidget::OnClientGameButtonClicked);
	}
}

void UQLobbyLevelWidget::ChangeButtonImage(UTexture2D* NewTexture)
{
	if (!GameButton || !NewTexture) return;

	// 브러시 생성
	FSlateBrush NewBrush;
	NewBrush.SetResourceObject(NewTexture);
	NewBrush.ImageSize = FVector2D(300.f, 100.f); // 원본 텍스처 해상도에 맞게 조절

	// 기존 스타일 가져오기 → 수정 → 적용
	FButtonStyle ButtonStyle;
	ButtonStyle.Normal = NewBrush;
	ButtonStyle.Hovered = NewBrush;
	ButtonStyle.Pressed = NewBrush;

	// 스타일 다시 세팅
	GameButton->SetStyle(ButtonStyle);
}

void UQLobbyLevelWidget::OnHostGameButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("OnHostGameButtonClicked"));
	AQGameModeLobby* LobbyGM = Cast<AQGameModeLobby>(UGameplayStatics::GetGameMode(this));
	if (LobbyGM && LobbyGM->HasAuthority())
	{
		LobbyGM->HostGameStart();
	}
}

void UQLobbyLevelWidget::OnClientGameButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("OnClientGameButtonClicked"));

	bIsReady = !bIsReady;
	UTexture2D* LoadedTexture;
	if (bIsReady)
	{
		LoadedTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Art/UI/NotReady.NotReady"));
	}
	else
	{
		LoadedTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Art/UI/Ready.Ready"));
	}
	ChangeButtonImage(LoadedTexture);
	
	AQLobbyPlayerController* PC = Cast<AQLobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->ServerRPC_ToggleClientReady(); // ✅ PlayerController → 서버로 요청
	}
}

void UQLobbyLevelWidget::UpdateHostButton(bool IsEnabled)
{
	GameButton->SetIsEnabled(IsEnabled);
}

void UQLobbyLevelWidget::UpdateClientButton(bool IsEnabled)
{
	GameButton->SetIsEnabled(IsEnabled);
}

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

void UQLobbyLevelWidget::AppendLogMessage(const FString& Message)
{
	if (LogText)
	{
		FString Current = LogText->GetText().ToString();
		LogText->SetText(FText::FromString(Current + Message + TEXT("\n")));
	}
}

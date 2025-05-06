// Copyright_Team_AriAri


#include "Game/QVillageGameState.h"

#include <string>

#include "QGameModeVillage.h"
#include "QLogCategories.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/QPlayerState.h"
#include "Player/QPlayerController.h"
#include "UI/QVillageTimerWidget.h"
#include "UI/QVillageUIManager.h"
#include "UI/QDefaultVillageWidget.h"
#include "UObject/ConstructorHelpers.h"


AQVillageGameState::AQVillageGameState()
{
	bReplicates = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("/Game/Blueprints/UI/WBP_ConversationTest"));
	if (WidgetFinder.Succeeded())
	{
		StartLevelWidget = WidgetFinder.Class;
	}

	// 초기화
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AQVillageGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority()) // 서버에서만 실행
	{
		ServerLeftTimeUntilTrial += DeltaSeconds;
		ForceNetUpdate();

		MulticastRPCUpdateServerTime();
		if (GetNetMode() == NM_ListenServer) // ✅ Listen 서버의 "호스트 클라이언트"에서는 직접 실행
		{
			MulticastRPCUpdateServerTime_Implementation();
		}
		
	}
}

void AQVillageGameState::EndVillageActivity_Implementation()
{
	//UE_LOG(LogLogic, Log, TEXT("AQVillageGameState::EndVillageActivity_Implementation : 아직 미구현상태"));
	
	AQVillageUIManager::GetInstance(GetWorld())->MulticastEndupUI();
	//플레이어정리 : 로컬플레이어의 상호작용 차단
	APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController();
	if (LocalPlayerController) {
		if (AQPlayerController* QPlayerController = Cast<AQPlayerController>(LocalPlayerController)) {
			QPlayerController->MulticastBlockInteraction();
			//UI정리
			//UI상호작용차단
			TObjectPtr<AQVillageUIManager> VillageUIManager =  QPlayerController->GetVillageUIManager();
			VillageUIManager->MulticastEndupUI();
			//안내문구띄우기
			Cast<UQDefaultVillageWidget>(VillageUIManager->GetActivedVillageWidgets()[EVillageUIType::DefaultVillageUI])->TurnOnGrandTitle();
			Cast<UQDefaultVillageWidget>(VillageUIManager->GetActivedVillageWidgets()[EVillageUIType::DefaultVillageUI])->TurnOnMiddleTitle();
			Cast<UQDefaultVillageWidget>(VillageUIManager->GetActivedVillageWidgets()[EVillageUIType::DefaultVillageUI])->SetGrandTitle(FText::FromString(TEXT("시간 종료!")));
			Cast<UQDefaultVillageWidget>(VillageUIManager->GetActivedVillageWidgets()[EVillageUIType::DefaultVillageUI])->SetMiddleTitle(FText::FromString(TEXT("곧 재판장으로 이동합니다.")));
			
		}
		else {
			UE_LOG(LogLogic, Error, TEXT("AQVillageGameState::EndVillageActivity_Implementation : QPlayerController 캐스팅 실패"));
		}
	}
}

void AQVillageGameState::MulticastRPCUpdateServerTime_Implementation()
{
	TObjectPtr<UQVillageTimerWidget> VillageTimerUI = Cast<UQVillageTimerWidget>(AQVillageUIManager::GetInstance(GetWorld())->GetActivedVillageWidgets()[EVillageUIType::VillageTimer]);
	if (VillageTimerUI)
	{
		VillageTimerUI->UpdateServerTimeToUITime(ServerLeftTimeUntilTrial, TimeUntilTrialMax);
	}
	else
	{
		UE_LOG(LogLogic, Log, TEXT("Get VillageTimerUI failed"));
	}
}

void AQVillageGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AQVillageGameState, ServerLeftTimeUntilTrial);
}

// 재판장 이동 함수 ------------------------------------------------------------------------------------------
void AQVillageGameState::ServerRPCRequestTravelToCourt_Implementation(APlayerController* LocalPlayerController, bool bTravelToCourt)
{
	if (!HasAuthority()) return;

	// todo: 해당 클라이언트가 재판장으로 이동할 준비가 되었다고 Gamemode에 업데이트
}

void AQVillageGameState::BeginPlay()
{
	Super::BeginPlay();
	// 이부분을 주석해제하면 테스트 결과 확인가능//
	/*
	UUserWidget* StartWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), StartLevelWidget);
	if (StartWidget)
	{
		// 위젯을 화면에 추가
		StartWidget->AddToViewport();
	}
	*/
}

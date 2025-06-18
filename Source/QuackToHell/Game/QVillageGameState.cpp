// Copyright_Team_AriAri


#include "Game/QVillageGameState.h"
#include "QGameModeVillage.h"
#include "QLogCategories.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Player/QPlayerState.h"
#include "Kismet/GameplayStatics.h"
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
		if (!bIsTimeToGoToCourt) {
			// 종료 시간이 다 됐다면 클라이언트들에게 마무리 작업하라고 알림.
			if (ServerLeftTimeUntilTrial > TimeUntilTrialMax)
			{
				bIsTimeToGoToCourt = true;
				// @todo : 유진의 클라 인터페 여기서 호출
				EndVillageActivity();
			}
		}
		
		ServerLeftTimeUntilTrial += DeltaSeconds;
		MulticastRPCUpdateServerTime();
	}
}

void AQVillageGameState::EndVillageActivity_Implementation()
{
	//UE_LOG(LogLogic, Log, TEXT("AQVillageGameState::EndVillageActivity_Implementation : 아직 미구현상태"));
	
	AQVillageUIManager::GetInstance(GetWorld())->MulticastEndupUI();
	//플레이어정리 : 로컬플레이어의 상호작용 차단
	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (LocalPlayerController) {
		if (AQPlayerController* QPlayerController = Cast<AQPlayerController>(LocalPlayerController)) {
			QPlayerController->MulticastBlockInteraction();
			//UI정리
			//UI상호작용차단
			TObjectPtr<AQVillageUIManager> VillageUIManager =  QPlayerController->GetVillageUIManager();
			VillageUIManager->MulticastEndupUI();
			//안내문구띄우기
			Cast<UQDefaultVillageWidget>(VillageUIManager->GetActivedWidget(EVillageUIType::DefaultVillageUI))->TurnOnGrandTitle();
			Cast<UQDefaultVillageWidget>(VillageUIManager->GetActivedWidget(EVillageUIType::DefaultVillageUI))->TurnOnMiddleTitle();
			Cast<UQDefaultVillageWidget>(VillageUIManager->GetActivedWidget(EVillageUIType::DefaultVillageUI))->SetGrandTitle(FText::FromString(TEXT("시간 종료!")));
			Cast<UQDefaultVillageWidget>(VillageUIManager->GetActivedWidget(EVillageUIType::DefaultVillageUI))->SetMiddleTitle(FText::FromString(TEXT("곧 재판장으로 이동합니다.")));
			
			// 플레이어 컨트롤러를 얻어서
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (AQPlayerController* QPC = Cast<AQPlayerController>(PC))
			{
				// ③ **클라이언트 타이머 시작**을 요청
				QPC->Client_StartCourtTravelTimer();
			}
		}
		else {
			UE_LOG(LogLogic, Error, TEXT("AQVillageGameState::EndVillageActivity_Implementation : QPlayerController 캐스팅 실패"));
		}
	}
}

void AQVillageGameState::MulticastRPCUpdateServerTime_Implementation()
{
	
	//위젯 가져오고, 시간 업데이트.
	TObjectPtr<UQVillageTimerWidget> VillageTimerUI = Cast<UQVillageTimerWidget>(AQVillageUIManager::GetInstance(GetWorld())->GetActivedWidget(EVillageUIType::VillageTimer));
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
	DOREPLIFETIME(AQVillageGameState, TimeUntilTrialMax);
	DOREPLIFETIME(AQVillageGameState, bIsTimeToGoToCourt);
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

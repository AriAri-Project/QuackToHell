// Copyright_Team_AriAri


#include "Game/QGameModeVillage.h"
#include "QGameInstanceVillage.h"
#include "QVillageGameState.h"
#include "Player/QPlayerState.h"
#include "UObject/ConstructorHelpers.h"
#include "GodCall.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "JuryComponent.h"
#include "QLogCategories.h"
#include "ResidentComponent.h"
#include "Player/QPlayerController.h"


AQGameModeVillage::AQGameModeVillage()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerClassRef(TEXT("/Game/Blueprints/Character/BP_QLaywer.BP_QLaywer_C"));
	if (PlayerClassRef.Class) {	// ThirdPersonClassRef.Class가 null이 아니라면
		DefaultPawnClass = PlayerClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerRef(TEXT("/Game/Blueprints/Player/BP_QPlayerController.BP_QPlayerController_C"));
	if (PlayerControllerRef.Class)
	{
		PlayerControllerClass = PlayerControllerRef.Class;
	}
	GameStateClass = AQVillageGameState::StaticClass();
	PlayerStateClass = AQPlayerState::StaticClass();
}

void AQGameModeVillage::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UE_LOG(LogTemp, Log, TEXT("📌 PostInitializeComponents() 실행됨!"));
	/*UE_LOG(LogTemp, Log, TEXT("QGameModeVillage::PostInitializeComponents() - 프롬프트 미리 준비 시작"));

	GeneratePromptsBeforeNextGame();*/
}

void AQGameModeVillage::BeginPlay()
{
	Super::BeginPlay();
	
	// NPC ID 카운터 리셋
	UJuryComponent::JuryCount = 0;
	UResidentComponent::ResidentCount = 0;
	UE_LOG(LogTemp, Log, TEXT("NPCID 카운터 초기화 완료 (JuryCount = 0, ResidentCount = 0)"));


	UE_LOG(LogTemp, Log, TEXT("AAIGameModeTest::BeginPlay() 실행됨"));

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ BeginPlay - World is nullptr!"));
		return;
	}

	// 기존 GodActor 찾기
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(World, TEXT("GodActor"), FoundActors);

	if (FoundActors.Num() == 0 || !FoundActors[0])
	{
		UE_LOG(LogTemp, Error, TEXT("❌ BeginPlay - GodActor를 찾을 수 없음!"));
		return;
	}

	AActor* GodActor = FoundActors[0];
	UGodCall* GodCall = GodActor->FindComponentByClass<UGodCall>();

	if (!GodCall)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ BeginPlay - GodCall을 찾을 수 없음!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("✅ BeginPlay - 프롬프트 생성 완료 후 게임 진행"));
}

void AQGameModeVillage::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogTemp, Log, TEXT("QGameModeVillage::EndPlay() - 프롬프트 삭제 및 재생성 예약"));

	UQGameInstanceVillage* GameInstance = Cast<UQGameInstanceVillage>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SchedulePromptRegeneration();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ GameInstance를 찾을 수 없음! 프롬프트 재생성 실패"));
	}

}

void AQGameModeVillage::TravelToCourtMap()
{
	if (!HasAuthority()) return;

	// 모든 플레이어가 준비되지 않았다면 return
	for (TActorIterator<AQPlayerState> IT(GetWorld()); IT; ++IT)
	{
		if (!IT->GetIsReadyToTravelToCourt())
		{
			UE_LOG(LogLogic, Log, TEXT("AQVillageGameState::ServerRPCRequestTravelToCourt_Implementation - %s is not ready for travel to court."), *IT->GetName());
			return;
		}
	}
	
	// 1) GameMode 에서 seamless travel 켜기
	bUseSeamlessTravel = true;

	// 2) 서버 주도 맵 전환 (listen 모드 유지)
	GetWorld()->ServerTravel(TEXT("/Game/Maps/CourtMap?listen"));
}

void AQGameModeVillage::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	// 기존 Controller를 새로운 Controller로 교체
	APlayerController* OldPC = Cast<APlayerController>(C);
	if (OldPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Replacing PlayerController %s with VillageController"), *OldPC->GetName());

		// 복사해 둘 정보
		FVector OldLocation = OldPC->GetPawn() ? OldPC->GetPawn()->GetActorLocation() : FVector::ZeroVector;
		FRotator OldRotation = OldPC->GetPawn() ? OldPC->GetPawn()->GetActorRotation() : FRotator::ZeroRotator;
		FString PlayerName = OldPC->PlayerState ? OldPC->PlayerState->GetPlayerName() : TEXT("Unknown");

		// 기존 컨트롤러 제거
		OldPC->Destroy();

		// 새 컨트롤러 생성
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = nullptr;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AQPlayerController* NewPC = GetWorld()->SpawnActor<AQPlayerController>(AQPlayerController::StaticClass(), OldLocation, OldRotation, SpawnParams);

		NewPC->PlayerState->SetPlayerName(PlayerName);
		C = NewPC;  // 중요: 교체된 컨트롤러를 참조로 되돌려 줘야 함

		// 새 Pawn 스폰
		RestartPlayer(NewPC);
	}
}
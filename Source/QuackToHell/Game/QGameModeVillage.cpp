// Copyright_Team_AriAri


#include "Game/QGameModeVillage.h"
#include "QGameInstanceVillage.h"

#include "QVillageGameState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Player/QPlayerState.h"
#include "UObject/ConstructorHelpers.h"
#include "GodCall.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

// 정적(static) 변수 정의
int32 AQGameModeVillage::PlayerIDCount;
int32 AQGameModeVillage::NPCIDCount;
int32 AQGameModeVillage::EvidenceIDCount;
int32 AQGameModeVillage::ConversationIDCount;

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
	
	// 정적(static) 변수는 클래스 차원에서 관리되므로, 생성자에서 값을 설정
	PlayerIDCount = PlayerIDInit;
	NPCIDCount = NPCIDInit;
	EvidenceIDCount = EvidenceIDInit;
	ConversationIDCount = ConversationIDInit;
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

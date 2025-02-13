// Copyright_Team_AriAri


#include "UI/QVillageUIManager.h"

#include "EngineUtils.h"
#include "UI/QP2NWidget.h"
#include "UI/QDefaultVillageWidget.h"
#include "UI/QRecordWidget.h"
#include "UI/QVillageTimerWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/QChatBoxWidget.h"
#include "UI/QMapWidget.h"
#include "UI/QPlayer2NSpeechBubbleWidget.h"
#include "UI/QSpeechBubbleWidget.h"
#include "UI/QRecordWidget.h"
#include "QLogCategories.h"
#include "UI/QInventoryWidget.h"

//(이중포인터아님)클래스타입 재차 명시한 이유: 어떤 클래스의 정적 멤버인지 명확히 지정" 하기 위함(C++문법)
TObjectPtr<AQVillageUIManager> AQVillageUIManager::Instance = nullptr;

// Sets default values
AQVillageUIManager::AQVillageUIManager()
{
	if (!IsVillageMap()) {
		UE_LOG(LogLogic, Warning, TEXT("마을맵이 아니어서 VillageUIManager를 생성할 수 없습니다."));
		return;
	}
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

	//UI타입과 UI클래스 매핑
	/**
	 * @TODO: 마을 내 유아이들 전부 추가해주기
	 */
	static ConstructorHelpers::FClassFinder<UQP2NWidget> P2NWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QP2NWidget.WBP_QP2NWidget_C'"));
	static ConstructorHelpers::FClassFinder<UQDefaultVillageWidget> DefaultVillageWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QDefailtVillageWidgets.WBP_QDefailtVillageWidgets_C'"));
	static ConstructorHelpers::FClassFinder<UQMapWidget> MapWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QMap.WBP_QMap_C'"));
	static ConstructorHelpers::FClassFinder<UQPlayer2NSpeechBubbleWidget> Player2NSpeechBubbleWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QPlayer2NSpeechBubble.WBP_QPlayer2NSpeechBubble_C'"));
	static ConstructorHelpers::FClassFinder<UQSpeechBubbleWidget> SpeechBubbleWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QSpeechBubble.WBP_QSpeechBubble_C'"));
	static ConstructorHelpers::FClassFinder<UQInventoryWidget> InventoryWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QInventory.WBP_QInventory_C'"));
	static ConstructorHelpers::FClassFinder<UQRecordWidget> RecordWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_Record.WBP_Record_C'"));
	static ConstructorHelpers::FClassFinder<UQVillageTimerWidget> VillageTimerWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_VillageTimer.WBP_VillageTimer_C'"));


	// TSubclassOf 템플릿 클래스 객체에 블루프린트 클래스를 넣어준다
	if (P2NWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(EVillageUIType::P2N, P2NWidgetAsset.Class);
	}
	if (DefaultVillageWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(EVillageUIType::DefaultVillageUI, DefaultVillageWidgetAsset.Class);
	}
	if (MapWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(EVillageUIType::Map, MapWidgetAsset.Class);
	}
	if (Player2NSpeechBubbleWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(EVillageUIType::Player2NSpeechBubble, Player2NSpeechBubbleWidgetAsset.Class);
	}
	if (SpeechBubbleWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(EVillageUIType::SpeechBubble, SpeechBubbleWidgetAsset.Class);
	}
	if (InventoryWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(EVillageUIType::Inventory, InventoryWidgetAsset.Class);
	}
	if (RecordWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(EVillageUIType::Record, RecordWidgetAsset.Class);
	}
	if (VillageTimerWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(EVillageUIType::VillageTimer, VillageTimerWidgetAsset.Class);
	}

}

void AQVillageUIManager::TurnOffUI(EVillageUIType UIType)
{
	if (ActivedVillageWidgets.Contains(UIType)) {
		//안 보이게 바꾼다.
		ActivedVillageWidgets[UIType]->SetVisibility(ESlateVisibility::Hidden);
	}
}

TObjectPtr<AQVillageUIManager> AQVillageUIManager::GetInstance(TObjectPtr<UWorld> World)
{
	if (!Instance)
	{
		// ✅ 클라이언트에서도 `AQVillageUIManager` 찾도록 설정
		for (TActorIterator<AQVillageUIManager> It(World); It; ++It)
		{
			Instance = *It;
			break;
		}

		// ✅ 그래도 없으면 생성 (서버에서만 실행)
		if (!Instance && World->GetNetMode() != NM_Client)
		{
			Instance = World->SpawnActor<AQVillageUIManager>(AQVillageUIManager::StaticClass());
		}
	}
	return Instance;
}

TMap<EVillageUIType, TObjectPtr<UUserWidget>> AQVillageUIManager::GetActivedVillageWidgets() const
{
	return ActivedVillageWidgets;
}

// Called when the game starts or when spawned
void AQVillageUIManager::BeginPlay()
{
	Super::BeginPlay();
	if (!Instance) {
		Instance = this;
	}
	//UI초기화
	if (GetNetMode() == NM_Client || HasAuthority())
	{
		OnMapLoad();
	}
	if (!HasAuthority())
	{
		UE_LOG(LogLogic, Log, TEXT("Client -> AQVillageUIManager::BeginPlay()"));
	}
	else
	{
		UE_LOG(LogLogic, Log, TEXT("Server -> AQVillageUIManager::BeginPlay()"));
	}
	
}

void AQVillageUIManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Instance == this) {
		Instance = nullptr;//싱글톤 해제 (싱글톤이 ObjectPtr이므로 객체는 GC에 수거됨)
		UE_LOG(LogLogic, Log, TEXT("QVillageManager: instance해제완료"));
	}
}




void AQVillageUIManager::OnMapLoad()
{
	//마을맵이면 기본으로 활성화 할 것들
	/**
	* @TODO: 기본 마을 UI들 띄운다(초기화작업).
	* 
	*/
	TurnOnUI(EVillageUIType::DefaultVillageUI);
	UE_LOG(LogLogic, Log, TEXT("UIManager - 디폴트위젯: %s"), *ActivedVillageWidgets[EVillageUIType::DefaultVillageUI].GetName());

	if (!HasAuthority())
	{
		UE_LOG(LogLogic, Log, TEXT("Client -> AQVillageUIManager::OnMapLoad()"));
	}
	else
	{
		UE_LOG(LogLogic, Log, TEXT("Server -> AQVillageUIManager::OnMapLoad()"));
	}
}

bool AQVillageUIManager::IsVillageMap()
{
	FString CurrentMap = UGameplayStatics::GetCurrentLevelName(this);
	/**
	 * @todo 마을맵으로 변경.
	 * 
	 * 
	 */
	if (CurrentMap == "VillageMap") {
		return true;
	}
	return false;
}


// Called every frame
void AQVillageUIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQVillageUIManager::TurnOnUI(EVillageUIType UIType)
{
	/*예외처리*/
	//키에 매핑되는 클래스목록이 존재하지 않으면
	if (!UIWidgetClasses.Contains(UIType)) {
		UE_LOG(LogLogic, Warning, TEXT("QVillageUIManager: 키에 매핑되는 클래스목록이 존재하지 않습니다."));
		return;
	}

	/*UI켜기*/
	//위젯 이미 만들어져있으면 visible 전환
	if (ActivedVillageWidgets.Contains(UIType)) {
		ActivedVillageWidgets[UIType]->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	//없으면 위젯 새로 만들기 & visible처리
	TObjectPtr<UWorld> World = GetWorld();
	if (!World) return;
	
	TObjectPtr<UUserWidget> NewWidget = CreateWidget<UUserWidget>(World, UIWidgetClasses[UIType]);
	if (NewWidget) {
		NewWidget->AddToViewport();
		NewWidget->SetVisibility(ESlateVisibility::Visible);  
		ActivedVillageWidgets.Add(UIType, NewWidget);
	}
}




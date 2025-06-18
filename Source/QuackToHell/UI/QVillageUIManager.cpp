// Copyright_Team_AriAri


#include "UI/QVillageUIManager.h"

#include "EngineUtils.h"
#include "UI/QP2NWidget.h"
#include "QLogCategories.h"
#include "UI/QDefaultVillageWidget.h"
#include "UI/QEvidenceWidget.h"
#include "UI/QRecordWidget.h"
#include "UI/QVillageTimerWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/QChatBoxWidget.h"
#include "UI/QMapWidget.h"
#include "UI/QPlayer2NSpeechBubbleWidget.h"
#include "UI/QSpeechBubbleWidget.h"
#include "UI/QRecordWidget.h"
#include "UI/QDefaultVillageWidget.h"
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

	static ConstructorHelpers::FClassFinder<UQP2NWidget> P2NWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QP2NWidget.WBP_QP2NWidget_C'"));
	static ConstructorHelpers::FClassFinder<UQDefaultVillageWidget> DefaultVillageWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QDefailtVillageWidgets.WBP_QDefailtVillageWidgets_C'"));
	static ConstructorHelpers::FClassFinder<UQMapWidget> MapWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QMap.WBP_QMap_C'"));
	static ConstructorHelpers::FClassFinder<UQPlayer2NSpeechBubbleWidget> Player2NSpeechBubbleWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QPlayer2NSpeechBubble.WBP_QPlayer2NSpeechBubble_C'"));
	static ConstructorHelpers::FClassFinder<UQSpeechBubbleWidget> SpeechBubbleWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QSpeechBubble.WBP_QSpeechBubble_C'"));
	static ConstructorHelpers::FClassFinder<UQInventoryWidget> InventoryWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_QInventory.WBP_QInventory_C'"));
	static ConstructorHelpers::FClassFinder<UQRecordWidget> RecordWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_Record.WBP_Record_C'"));
	static ConstructorHelpers::FClassFinder<UQVillageTimerWidget> VillageTimerWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_VillageTimer.WBP_VillageTimer_C'"));
	static ConstructorHelpers::FClassFinder<UQEvidenceWidget> EvidenceWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_EvidenceInventory.WBP_EvidenceInventory_C'"));


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
	if (EvidenceWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(EVillageUIType::Evidence, EvidenceWidgetAsset.Class);
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
	if (!World)
	{
		UE_LOG(LogLogic, Warning, TEXT("AQVillageUIManager::GetInstance - World is null"));
		return nullptr;
	}

	ENetMode NetMode = World->GetNetMode();
	// 디버그용 로그
	//UE_LOG(LogLogic, Log, TEXT("AQVillageUIManager::GetInstance - NetMode=%d"), NetMode);

	if (!Instance)
	{
		// DedicatedServer, ListenServer, Client, Standalone 모두 허용
		if (NetMode == NM_DedicatedServer
			|| NetMode == NM_ListenServer
			|| NetMode == NM_Client
			|| NetMode == NM_Standalone)
		{
			//UE_LOG(LogLogic, Log, TEXT("AQVillageUIManager 생성 (NetMode=%d)"), NetMode);
			Instance = World->SpawnActor<AQVillageUIManager>(AQVillageUIManager::StaticClass());
			if (!Instance)
			{
				UE_LOG(LogLogic, Error, TEXT("AQVillageUIManager 생성 실패!"));
			}
		}
		else
		{
			UE_LOG(LogLogic, Warning, TEXT("AQVillageUIManager::GetInstance - 생성 불가 NetMode=%d"), NetMode);
		}
	}

	return Instance;
}


UUserWidget* AQVillageUIManager::CreateWidgetIfNotExists(EVillageUIType UIType)
{
	/*예외처리*/
	//키에 매핑되는 클래스목록이 존재하지 않으면 강제종료
	if (!UIWidgetClasses.Contains(UIType)) {
		UE_LOG(LogLogic, Warning, TEXT("QVillageUIManager: 키에 매핑되는 클래스목록이 존재하지 않습니다."));
		FGenericPlatformMisc::RequestExit(true);
	}

	//이미 있으면, nullptr 리턴
	if (ActivedVillageWidgets.Contains(UIType))
	{
		return nullptr;
	}

	//World 없으면 강제종료
	TObjectPtr<UWorld> World = GetWorld();
	if (!World) {
		UE_LOG(LogLogic, Warning, TEXT("QVillageUIManager: World가 존재하지 않습니다."));
		FGenericPlatformMisc::RequestExit(true);
	}

	/*위젯 없으면 새로 만들기 */
	TObjectPtr<UUserWidget> NewWidget = CreateWidget<UUserWidget>(World, UIWidgetClasses[UIType]);
	if (NewWidget) {
		NewWidget->AddToViewport();
		NewWidget->SetVisibility(ESlateVisibility::Hidden);
		ActivedVillageWidgets.Add(UIType, NewWidget);
		return NewWidget;
	}

	// 위젯 생성 실패 시 
	UE_LOG(LogLogic, Warning, TEXT("QVillageUIManager: 위젯 생성에 실패했습니다."));
	FGenericPlatformMisc::RequestExit(true);
	return nullptr;
}


UUserWidget* AQVillageUIManager::GetActivedWidget(EVillageUIType UIType)
{
	/* 위젯이 없을때만 만들도록 : 반드시 유효한 UI 접근하게 보장 */
	CreateWidgetIfNotExists(UIType);
	// 위에 따라 있는거 보장되니, 리턴해주면 됨
	return ActivedVillageWidgets[UIType];

}

void AQVillageUIManager::CloseUIInteraction()
{
	//defaultUI에서 열어준 함수를 호출한다: 버튼 상호작용 막기
	Cast<UQDefaultVillageWidget>(ActivedVillageWidgets[EVillageUIType::DefaultVillageUI])->BlockButtonsInteraction();
}


void AQVillageUIManager::MulticastEndupUI_Implementation()
{
	UE_LOG(LogLogic, Log, TEXT("AQVillageUIManager::EndupUI: 미구현"));
	//1. 열린 UI팝업을 닫는다. == default UI빼고 다 turnoff한다. 
	for (auto& Widget : ActivedVillageWidgets) {
		if (Widget.Key != EVillageUIType::DefaultVillageUI) {
			Widget.Value->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	//2. 상호작용을 막는다.
	CloseUIInteraction();
}

// Called when the game starts or when spawned
void AQVillageUIManager::BeginPlay()
{
	Super::BeginPlay();
	if (!Instance) {
		Instance = this;
	}
	//UI초기화
	OnMapLoad();
	
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




void AQVillageUIManager::TurnOnUI(EVillageUIType UIType)
{
	/* 위젯이 없을때만 만들도록 : 반드시 유효한 UI 접근하게 보장 */
	CreateWidgetIfNotExists(UIType);
	// 위에 따라 있는 거 보장되니 visible 처리하면 됨
	ActivedVillageWidgets[UIType]->SetVisibility(ESlateVisibility::Visible);
}




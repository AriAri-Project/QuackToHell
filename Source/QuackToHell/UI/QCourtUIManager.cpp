// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QCourtUIManager.h"
#include "Kismet/GameplayStatics.h"
#include "QLogCategories.h"
#include "UI/QCourtTimerWidget.h"
#include "UI/QCourtInputBoxWidget.h"
#include "UI/QOpeningStatementWidget.h"
#include "EngineUtils.h"
//(이중포인터아님)클래스타입 재차 명시한 이유: 어떤 클래스의 정적 멤버인지 명확히 지정" 하기 위함(C++문법)
TObjectPtr<AQCourtUIManager> AQCourtUIManager::Instance = nullptr;

// Sets default values
AQCourtUIManager::AQCourtUIManager()
{

	if (!IsCourtMap()) {
		UE_LOG(LogLogic, Warning, TEXT("재판장맵이 아니어서 CourtUIManager를 생성할 수 없습니다."));
		return;
	}
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

	//UI타입과 UI클래스 매핑
	/**
	 * @TODO: 재판장 내 유아이들 전부 추가해주기
	 */
	static ConstructorHelpers::FClassFinder<UQOpeningStatementWidget> OpeningStatementWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_OpeningStatement.WBP_OpeningStatement_C'"));
	static ConstructorHelpers::FClassFinder<UQCourtTimerWidget> UQCourtTimerWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_CourtTImer.WBP_CourtTImer_C'"));
	static ConstructorHelpers::FClassFinder<UQCourtInputBoxWidget> UQCourtInputBoxWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_CourtInputBox.WBP_CourtInputBox_C'"));


	// TSubclassOf 템플릿 클래스 객체에 블루프린트 클래스를 넣어준다
	if (OpeningStatementWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(ECourtUIType::OpeningStatement, OpeningStatementWidgetAsset.Class);
	}
	if (UQCourtTimerWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(ECourtUIType::CourtTimer, UQCourtTimerWidgetAsset.Class);
	}
	if (UQCourtInputBoxWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(ECourtUIType::InputBox, UQCourtInputBoxWidgetAsset.Class);
	}


}

void AQCourtUIManager::OnMapLoad()
{
	//재판장맵이면 기본으로 활성화 할 것들
	/**
	* @TODO: 기본 마을 UI들 띄운다(초기화작업).
	*
	*/
	//기본 띄울 것 띄우기.
	// 블라블라 . . 

	if (!HasAuthority())
	{
		UE_LOG(LogLogic, Log, TEXT("Client -> AQVillageUIManager::OnMapLoad()"));
	}
	else
	{
		UE_LOG(LogLogic, Log, TEXT("Server -> AQVillageUIManager::OnMapLoad()"));
	}
}

bool AQCourtUIManager::IsCourtMap()
{
	FString CurrentMap = UGameplayStatics::GetCurrentLevelName(this);
	/**
	 * @todo 재판장맵으로 변경.
	 *
	 *
	 */
	/*if (CurrentMap == "CourtMap") {
		return true;
	}
	return false;*/

	return true;
}

void AQCourtUIManager::TurnOnUI(ECourtUIType UIType)
{
	/*예외처리*/
	//키에 매핑되는 클래스목록이 존재하지 않으면
	if (!UIWidgetClasses.Contains(UIType)) {
		UE_LOG(LogLogic, Warning, TEXT("QCourtUIManager: 키에 매핑되는 클래스목록이 존재하지 않습니다."));
		return;
	}

	/*UI켜기*/
	//위젯 이미 만들어져있으면 visible 전환
	if (ActivedCourtWidgets.Contains(UIType)) {
		ActivedCourtWidgets[UIType]->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	//없으면 위젯 새로 만들기 & visible처리
	TObjectPtr<UWorld> World = GetWorld();
	if (!World) return;

	TObjectPtr<UUserWidget> NewWidget = CreateWidget<UUserWidget>(World, UIWidgetClasses[UIType]);
	if (NewWidget) {
		NewWidget->AddToViewport();
		NewWidget->SetVisibility(ESlateVisibility::Visible);
		ActivedCourtWidgets.Add(UIType, NewWidget);
	}
}

void AQCourtUIManager::TurnOffUI(ECourtUIType UIType)
{
	if (ActivedCourtWidgets.Contains(UIType)) {
		//안 보이게 바꾼다.
		ActivedCourtWidgets[UIType]->SetVisibility(ESlateVisibility::Hidden);
	}
}

TObjectPtr<AQCourtUIManager> AQCourtUIManager::GetInstance(TObjectPtr<UWorld> World)
{
	if (!Instance)
	{
		// ✅ 클라이언트에서도 `AQVillageUIManager` 찾도록 설정
		for (TActorIterator<AQCourtUIManager> It(World); It; ++It)
		{
			Instance = *It;
			break;
		}

		// ✅ 그래도 없으면 생성 (서버에서만 실행)
		if (!Instance && World->GetNetMode() != NM_Client)
		{
			Instance = World->SpawnActor<AQCourtUIManager>(AQCourtUIManager::StaticClass());
		}
	}
	return Instance;
}

TMap<ECourtUIType, TObjectPtr<UUserWidget>> AQCourtUIManager::GetActivedCourtWidgets() const
{
	return ActivedCourtWidgets;
}

// Called when the game starts or when spawned
void AQCourtUIManager::BeginPlay()
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

void AQCourtUIManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Instance == this) {
		Instance = nullptr;//싱글톤 해제 (싱글톤이 ObjectPtr이므로 객체는 GC에 수거됨)
		UE_LOG(LogLogic, Log, TEXT("QCourtManager: instance해제완료"));
	}
}

// Called every frame
void AQCourtUIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AQCourtUIManager::TurnOnOpening_Implementation()
{
	UE_LOG(LogLogic, Log, TEXT("재판 시작 연출 ~~ 미구현"));
}


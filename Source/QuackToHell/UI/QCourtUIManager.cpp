// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QCourtUIManager.h"
#include "Kismet/GameplayStatics.h"
#include "QLogCategories.h"
#include "UI/QCourtTimerWidget.h"
#include "UI/QEvidenceExamWidget.h"
#include "UI/QCourtInputBoxWidget.h"
#include "UI/CourtOpeningDirection.h"
#include "UI/QOpeningStatementWidget.h"
#include "UI/QExamDefendantWidget.h"
#include "EngineUtils.h"
#include <Game/QGameStateCourt.h>
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
	static ConstructorHelpers::FClassFinder<UQEvidenceExamWidget> UQEvidenceExamWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_EvidenceExam.WBP_EvidenceExam_C'"));
	static ConstructorHelpers::FClassFinder<UQExamDefendantWidget> UQExamDefendantWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_ExamDefendant.WBP_ExamDefendant_C'"));
	/**
	 * @TODO: 재판장 내 연출들 전부 추가해주기
	 */
	static ConstructorHelpers::FClassFinder<UCourtOpeningDirection> OpeningDirectionWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_CourtOpeningDirection.WBP_CourtOpeningDirection_C'"));



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
	if (UQEvidenceExamWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(ECourtUIType::EvidenceExam, UQEvidenceExamWidgetAsset.Class);
	}
	if (UQExamDefendantWidgetAsset.Succeeded())
	{
		UIWidgetClasses.Add(ECourtUIType::ExamDefendant, UQExamDefendantWidgetAsset.Class);
	}


	/* 디렉션 */
	if (OpeningDirectionWidgetAsset.Succeeded())
	{
		DirectionWidgetClasses.Add(ECourtDirectionType::Opening, OpeningDirectionWidgetAsset.Class);
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


void AQCourtUIManager::ServerRPCAlertOpeningEnd_Implementation(bool isSucceeded)
{
	if (!HasAuthority() || !isSucceeded) return;

	if (UWorld* World = GetWorld())
	{
		
		if (AQGameStateCourt* GS = World->GetGameState<AQGameStateCourt>())
		{
			GS->MultiRPCStartUI(ECourtUIType::OpeningStatement);
			TurnOffDirection(ECourtDirectionType::Opening);
		}
	}
}



void AQCourtUIManager::ServerRPCAlerOpeningStatementInputEnd_Implementation(bool isSucceeded)
{
}



void AQCourtUIManager::ServerRPCAlerEvidenceExamInputEnd_Implementation(bool isSucceeded)
{
}



void AQCourtUIManager::ServerRPCAlertExamDefendantInputEnd_Implementation(bool isSucceeded)
{
}

void AQCourtUIManager::ServerRPCAlertDefendantAnswerEnd_Implementation(bool isSurcceded)
{
}

void AQCourtUIManager::ServerRPCAlertJuryOpinionPerformEnd_Implementation(bool isSurcceded)
{
}

void AQCourtUIManager::ServerRPCAlertFinalJudgementPerformEnd_Implementation(bool isSurcceded)
{
}

void AQCourtUIManager::ServerRPCRemoveUserFromSession_Implementation(APlayerController* LocalPlayerController)
{
}




void AQCourtUIManager::MultiRPCTurnOnDirection_Implementation(ECourtDirectionType DirectionType)
{
	/* 위젯이 없을때만 만들도록 : 반드시 유효한 UI 접근하게 보장 */
	CreateWidgetIfNotExists(DirectionType);
	// 위에 따라 있는 거 보장되니 visible 처리하면 됨
	ActivedCourtDirectionWidgets[DirectionType]->SetVisibility(ESlateVisibility::Visible);
}



void AQCourtUIManager::TurnOnUI(ECourtUIType UIType)
{
	/* 위젯이 없을때만 만들도록 : 반드시 유효한 UI 접근하게 보장 */
	CreateWidgetIfNotExists(UIType);
	// 위에 따라 있는 거 보장되니 visible 처리하면 됨
	ActivedCourtUIWidgets[UIType]->SetVisibility(ESlateVisibility::Visible);

}

void AQCourtUIManager::MultiRPCTurnOnUI_Implementation(ECourtUIType UIType)
{
	/* 위젯이 없을때만 만들도록 : 반드시 유효한 UI 접근하게 보장 */
	CreateWidgetIfNotExists(UIType);
	// 위에 따라 있는 거 보장되니 visible 처리하면 됨
	ActivedCourtUIWidgets[UIType]->SetVisibility(ESlateVisibility::Visible);
}

void AQCourtUIManager::TurnOffUI(ECourtUIType UIType)
{
	if (ActivedCourtUIWidgets.Contains(UIType)) {
		//안 보이게 바꾼다.
		ActivedCourtUIWidgets[UIType]->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AQCourtUIManager::TurnOffDirection_Implementation(ECourtDirectionType DirectionType)
{
	if (ActivedCourtDirectionWidgets.Contains(DirectionType)) {
		//안 보이게 바꾼다.
		ActivedCourtDirectionWidgets[DirectionType]->SetVisibility(ESlateVisibility::Hidden);
	}

}

TObjectPtr<AQCourtUIManager> AQCourtUIManager::GetInstance(TObjectPtr<UWorld> World)
{
	if (!Instance && World)
	{
		// ListenServer, Client, Standalone 모두에서 스폰
		// ListenServer, Client, Standalone 모두에서 스폰
		Instance = World->SpawnActor<AQCourtUIManager>(AQCourtUIManager::StaticClass());
		if (!Instance)
		{
			UE_LOG(LogLogic, Error, TEXT("AQCourtUIManager 스폰 실패"));
		}
	}
	return Instance;
}

TMap<ECourtUIType, TObjectPtr<UUserWidget>> AQCourtUIManager::GetActivedCourtWidgets() const
{
	return ActivedCourtUIWidgets;
}

// Called when the game starts or when spawned
void AQCourtUIManager::BeginPlay()
{
	Super::BeginPlay();

	// 클라이언트·서버 가리지 않고 자신을 싱글톤으로 등록
	if (!Instance)
	{
		Instance = this;
	}
	else
	{
		return; // 이미 다른 인스턴스가 있으면 두 번 초기화 금지
	}

	// 클라이언트에서도 OnMapLoad 호출하려면 여기에:
	OnMapLoad();

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

UUserWidget* AQCourtUIManager::GetActivedWidget(ECourtUIType UIType)
{
	return nullptr;
}

UUserWidget* AQCourtUIManager::GetActivedWidget(ECourtDirectionType UIType)
{
	return nullptr;
}

UUserWidget* AQCourtUIManager::CreateWidgetIfNotExists(ECourtUIType UIType)
{
	/*예외처리*/
	//키에 매핑되는 클래스목록이 존재하지 않으면 강제종료
	if (!UIWidgetClasses.Contains(UIType)) {
		UE_LOG(LogLogic, Warning, TEXT("QVillageUIManager: 키에 매핑되는 클래스목록이 존재하지 않습니다."));
		FGenericPlatformMisc::RequestExit(true);
	}

	//이미 있으면, nullptr 리턴
	if (ActivedCourtUIWidgets.Contains(UIType))
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
		ActivedCourtUIWidgets.Add(UIType, NewWidget);
		return NewWidget;
	}

	// 위젯 생성 실패 시 
	UE_LOG(LogLogic, Warning, TEXT("QVillageUIManager: 위젯 생성에 실패했습니다."));
	FGenericPlatformMisc::RequestExit(true);
	return nullptr;
}

UUserWidget* AQCourtUIManager::CreateWidgetIfNotExists(ECourtDirectionType UIType)
{
	/*예외처리*/
//키에 매핑되는 클래스목록이 존재하지 않으면 강제종료
	if (!DirectionWidgetClasses.Contains(UIType)) {
		UE_LOG(LogLogic, Warning, TEXT("QVillageUIManager: 키에 매핑되는 클래스목록이 존재하지 않습니다."));
		FGenericPlatformMisc::RequestExit(true);
	}

	//이미 있으면, nullptr 리턴
	if (ActivedCourtDirectionWidgets.Contains(UIType))
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
	TObjectPtr<UUserWidget> NewWidget = CreateWidget<UUserWidget>(World, DirectionWidgetClasses[UIType]);
	if (NewWidget) {
		NewWidget->AddToViewport();
		NewWidget->SetVisibility(ESlateVisibility::Hidden);
		ActivedCourtDirectionWidgets.Add(UIType, NewWidget);
		return NewWidget;
	}

	// 위젯 생성 실패 시 
	UE_LOG(LogLogic, Warning, TEXT("QVillageUIManager: 위젯 생성에 실패했습니다."));
	FGenericPlatformMisc::RequestExit(true);
	return nullptr;
}

void AQCourtUIManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// -------------------------------------------------------------------------------

// 모두진술 연출 끝났다고 서버가 받았을 때
void AQCourtUIManager::ServerRPCAlertOpeningStatementPerformEnd_Implementation(bool isSucceeded)
{
	if (!HasAuthority() || !isSucceeded) return;

	if (UWorld* World = GetWorld())
	{
		// GameState 에 “증거조사 도입” 연출 요청
		if (AQGameStateCourt* GS = World->GetGameState<AQGameStateCourt>())
		{
			//GS->ServerRPCStartDirection(ECourtDirectionType::EvidenceExamStarting);
		}
	}
}

// 증거조사 연출 끝났다고 서버가 받았을 때
void AQCourtUIManager::ServerRPCAlertEvidenceExamPerformEnd_Implementation(bool isSucceeded)
{
	if (!HasAuthority() || !isSucceeded) return;

	if (UWorld* World = GetWorld())
	{
		// GameState 에 “피고인 심문 도입” 연출 요청
		if (AQGameStateCourt* GS = World->GetGameState<AQGameStateCourt>())
		{
			//GS->ServerRPCStartDirection(ECourtDirectionType::ExamDefendantStarting);
		}
	}
}

// 피고인심문 연출 끝났다고 서버가 받았을 때
void AQCourtUIManager::ServerRPCAlertExamDefendantPerformEnd_Implementation(bool isSucceeded)
{
	if (!HasAuthority() || !isSucceeded) return;

	if (UWorld* World = GetWorld())
	{
		// GameState 에 “피고인 심문 응답” 연출 요청
		if (AQGameStateCourt* GS = World->GetGameState<AQGameStateCourt>())
		{
			//GS->ServerRPCStartDirection(ECourtDirectionType::ExamDefendantResponse);
		}
	}
}






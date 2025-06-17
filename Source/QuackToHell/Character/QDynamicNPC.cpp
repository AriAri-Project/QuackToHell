// Copyright_Team_AriAri


#include "Character/QDynamicNPC.h"

#include "NPCComponent.h"
#include "UI/QEKeyWidget.h"
#include "Character/QPlayer.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "QLogCategories.h"
#include "Kismet/GameplayStatics.h"
#include "Player/QPlayerController.h"
#include "NPC/QDynamicNPCController.h"
#include "UI/QPlayer2NSpeechBubbleWidget.h"
AQDynamicNPC::AQDynamicNPC(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	/*허공말풍선 UI 컴포넌트*/
	EWidgetSpace WidgetSpace = EWidgetSpace::Screen;
	this->Player2NSpeechBubbleWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Player2NSpeechBubbleWidget"));
	this->Player2NSpeechBubbleWidgetComponent->SetWidgetSpace(WidgetSpace);
	this->Player2NSpeechBubbleWidgetComponent->SetDrawAtDesiredSize(true);
	this->Player2NSpeechBubbleWidgetComponent->SetupAttachment(RootComponent);
	TSubclassOf<UQPlayer2NSpeechBubbleWidget> _Player2NSpeechBubbleWidget;
	//UQPlayer2NSpeechBubbleWidget을 상속한 클래스만 담을 수 있도록 강제한다.
	this->Player2NSpeechBubbleWidgetComponent->SetWidgetClass(_Player2NSpeechBubbleWidget);

	/*EKey UI 컴포넌트*/
	this->EKeyWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EKeyWidget"));
	this->EKeyWidgetComponent->SetWidgetSpace(WidgetSpace);
	this->EKeyWidgetComponent->SetDrawAtDesiredSize(true);
	this->EKeyWidgetComponent->SetupAttachment(RootComponent);
	TSubclassOf<UQEKeyWidget> _EKeyWidget;
	//QNameWidget을 상속한 클래스만 담을 수 있도록 강제한다.
	this->EKeyWidgetComponent->SetWidgetClass(_EKeyWidget);

}
TObjectPtr<AActor> AQDynamicNPC::GetClosestNPC()
{
	if (OverlappingNPCs.Num() == 0) {
		//대화 대상 없음
		return nullptr;
	}

	TObjectPtr<AActor> ClosestNPC = nullptr;
	float MinDistance = FLT_MAX;

	for (TObjectPtr<AActor> NPC : OverlappingNPCs) {
		//캐릭터와 NPC간 거리
		float Distance = FVector::Dist(this->GetActorLocation(), NPC->GetActorLocation());
		//최소거리찾기
		if (Distance < MinDistance) {
			MinDistance = Distance;
			ClosestNPC = NPC;
		}
	}

	return ClosestNPC;
}



bool AQDynamicNPC::GetResponse(AQPlayerController* ClientPC, FString& Text, EConversationType InputConversationType)
{
	// 응답 요청을 위한 FOpenAIRequest 구조체 구성
	FOpenAIRequest OpenAIRequest;
	OpenAIRequest.ConversationType = InputConversationType;
	AQPlayerController* PlayerController = Cast<AQPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController == nullptr)
	{
		UE_LOG(LogLogic, Error, TEXT("AQDynamicNPC::GetResponse - PlayerController is nullptr."));
		return false;
	}
	OpenAIRequest.ListenerID = Cast<AQPlayerState> (PlayerController->PlayerState)->GetPlayerId();
	OpenAIRequest.Prompt = Text;
	OpenAIRequest.SpeakerID = NPCComponent->GetNPCID();

	// 서버에게 AI 응답 요청 보내기
	NPCComponent->ServerRPCGetNPCResponse(ClientPC, OpenAIRequest);
	return true;
}

TObjectPtr<class UQPlayer2NSpeechBubbleWidget> AQDynamicNPC::GetPlayer2NSpeechBubbleWidget() const
{
	return Player2NSpeechBubbleWidget;
}

void AQDynamicNPC::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//만약 플레이어가 다가왔을 시, E대화하기 유아이 띄우기
	TObjectPtr<AQPlayer> OpponentPlayer = Cast<AQPlayer>(OtherActor);
	if (OpponentPlayer) {
		TurnOnEKeyUI();
	}

	//캐스팅 미성공시 nullptr
	TObjectPtr<AQDynamicNPC> OpponentNPC = Cast<AQDynamicNPC>(OtherActor);
	if (OpponentNPC) {
		//ClosestNPC를 찾기 위해 OverlappingNPCs에 추가
		OverlappingNPCs.Add(OpponentNPC);
	}

	//N2N : 만약 NPC가 다가왔을 시, 대화 시작하기
	//몇 초 뒤에 실행하도록 조치 취하기. : 1분  
	if (StartDialogTimer > StartDialogMaxTime) {
		OpponentNPC = Cast<AQDynamicNPC>(GetClosestNPC());
		if (OpponentNPC) {

			//조건 
			// 내가 대화기록 있으면 : 플레이어와 대화한 기록이 있는 망자NPC가 먼저[망자 NPC – 망자 NPC 대화말풍선 UI]를 나타낸다
			AQPlayerState* _PlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<AQPlayerState>();
			TArray<FConversationRecord> ConversationRecords = {};
			//플레이어스테이트로 대화기록 조회
			int32 NPCID = NPCComponent->GetNPCID();
			if (_PlayerState != nullptr)
			{
				ConversationRecords = _PlayerState->GetRecrodWithNPCID(NPCID);
			}
			else {
				UE_LOG(LogLogic, Error, TEXT("AQDynamicNPCController::StartDialog - PlayerState is nullptr"));
			}

			//대화기록이 있는지없는지 확인
			if (!ConversationRecords.IsEmpty()) {
				//대화기록 있는 애이니, 먼저 말 건다.
				AQDynamicNPCController* MyController = Cast<AQDynamicNPCController>(GetController());
				if (MyController == nullptr)
				{
					UE_LOG(LogLogic, Error, TEXT("AQDynamicNPC::OnOverlapBegin - Controller is nullptr."));
					return;
				}
				else {
					MyController->StartDialog(OpponentNPC, ENPCConversationType::N2N);
				}
			}
			else {
				//UE_LOG(LogLogic, Log, TEXT("AQDynamicNPCController::StartDialog - 플레이어와 대화기록이 없어 말을 걸 수 없습니다."));
			}
		}
	}

}

void AQDynamicNPC::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//만약 플레이어가 나갔을 시, E대화하기 유아이 끄기
	TObjectPtr<AQPlayer> OpponentPlayer = Cast<AQPlayer>(OtherActor);
	if (OpponentPlayer) {
		TurnOffEKeyUI();
	}
	
	
	//캐스팅 미성공시 nullptr
	TObjectPtr<AQDynamicNPC> OpponentNPC = Cast<AQDynamicNPC>(OtherActor);
	if (OpponentNPC) {
		OverlappingNPCs.Remove(OpponentNPC);
	}
}
void AQDynamicNPC::BeginPlay()
{
	Super::BeginPlay();
	/*EKey 위젯 변수에 객체값 할당*/
	if (EKeyWidgetComponent)
	{
		UUserWidget* UserWidget = EKeyWidgetComponent->GetWidget();
		if (UserWidget)
		{
			EKeyWidget = Cast<UQEKeyWidget>(UserWidget);
		}
	}

	/*Player2N말풍선 위젯 변수에 객체값 할당*/
	if (Player2NSpeechBubbleWidgetComponent)
	{
		UUserWidget* UserWidget = Player2NSpeechBubbleWidgetComponent->GetWidget();
		if (UserWidget)
		{
			Player2NSpeechBubbleWidget = Cast<UQPlayer2NSpeechBubbleWidget>(UserWidget);
		}
	}
}

void AQDynamicNPC::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	NPCComponent = FindComponentByClass<UNPCComponent>();
}

void AQDynamicNPC::Tick(float DeltaTime)
{
	/*대화시작 타이머*/
	if (StartDialogTimer <= StartDialogMaxTime) {
		StartDialogTimer += DeltaTime;
	}

}

void AQDynamicNPC::TurnOnEKeyUI()
{
	EKeyWidget->SetVisibility(ESlateVisibility::Visible);
}

void AQDynamicNPC::TurnOffEKeyUI()
{
	EKeyWidget->SetVisibility(ESlateVisibility::Hidden);
}

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
	OpponentNPC = Cast<AQDynamicNPC>(GetClosestNPC());
	if (OpponentNPC == nullptr)
	{
		UE_LOG(LogLogic, Log, TEXT("AQDynamicNPCController::StartDialog - Opponent NPC is nullptr."));
		return;
	}
	if (HasAuthority())
	{
		RequestConversationN2N(OpponentNPC);
		OpponentNPC->RequestConversationN2N(this);
	}
	
	
	/**
	if (StartDialogTimer > StartDialogMaxTime) {
		
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
				MyController->StartDialog(OpponentNPC, ENPCConversationType::N2N);
			}
			else {
				UE_LOG(LogLogic, Log, TEXT("AQDynamicNPCController::StartDialog - 플레이어와 대화기록이 없어 말을 걸 수 없습니다."));
			}
		}
	}
	**/
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

// ========================================================= //

void AQDynamicNPC::CountDownN2N()
{
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this]()
	{
		N2NCoolTimeCharged = true;
	});
	GetWorldTimerManager().SetTimer(N2NTimerHandle, TimerDel, 1.0f, false);
}

bool AQDynamicNPC::CheckCanStartConversN2N()
{
	if (!HasAuthority())
	{
		return false;
	}
	// NPC가 P2N, N2N 상호작용 중일 경우 N2N 대화 불가능
	if (this->GetNPCConversationState() == EConversationType::N2N || this->GetNPCConversationState() == EConversationType::P2N || this->GetNPCConversationState() == EConversationType::PStart)
	{
		return false;
	}
	if (!N2NCoolTimeCharged)
	{
		return false;
	}
	return true;
}

bool AQDynamicNPC::CheckCanFinishConversN2N()
{
	if (!HasAuthority())
	{
		false;
	}
	return true;
}

void AQDynamicNPC::RequestConversationN2N(AQDynamicNPC* TargetNPC)
{
	if (!HasAuthority())
	{
		return;
	}
	/* 체크할 것
	*	1. 대화에 참여하는 NPC가 P2N, N2N 상호작용 중일 경우 바로 종료
	*	2. NPC A 가 N2N 쿨타임이 60초 미만일 경우: 대화요청을 호출하지 않는다.
	*   3. NPC B 가 N2N 쿨타임이 60초 미만일 경우: 대화요청을 수락하지 않는다.
	*/

	// 상대 NPC가 nullptr이라면 바로 종료
	if (TargetNPC == nullptr)
	{
		UE_LOG(LogLogic, Error, TEXT("AQNPC::RequestConversationN2N - TargetNPC is nullptr."));
		return;
	}
	// @todo 둘 중 NPC ID가 더 낮은 쪽이 말을 건다.
	int32 MyNPCID = FindComponentByClass<UNPCComponent>()->GetNPCID();
	int32 TargetNPCID = TargetNPC->FindComponentByClass<UNPCComponent>()->GetNPCID();

	if (MyNPCID > TargetNPCID)
	{
		return;
	}

	// 두 NPC가 모두 N2N 대화가 가능한 지 check 후 가능하다면 AI에게 첫번째 대사 요청
	if (this->CheckCanStartConversN2N() && TargetNPC->CheckCanStartConversN2N())
	{
		// 두 NPC Freeze
		TObjectPtr<AQDynamicNPCController> DynamicNPCController;
		DynamicNPCController = Cast<AQDynamicNPCController>(this->GetController());
		DynamicNPCController->FreezePawn();
		DynamicNPCController = Cast<AQDynamicNPCController>(TargetNPC->GetController());
		DynamicNPCController->FreezePawn();
	
		// @todo AI에게 N2N의 첫번째 대화 생성 요청
		FOpenAIRequest Request(
		this->FindComponentByClass<UNPCComponent>()->GetNPCID(),
		TargetNPC->FindComponentByClass<UNPCComponent>()->GetNPCID(),
		EConversationType::N2NStart,
		""
		);
		this->FindComponentByClass<UNPCComponent>()->GetNPCResponse(DynamicNPCController, Request);
	}
	else
	{
		UE_LOG(LogLogic, Log, TEXT("AQNPC::RequestConversationN2N - NPC can't start N2N Conversation."));
	}
}

void AQDynamicNPC::StartConversationN2N(AQDynamicNPC* TargetNPC, FOpenAIResponse FirstResponse)
{
	if (!HasAuthority())
	{
		return;
	}
	// 1. 플레이어 시점 말풍선 처리
	TObjectPtr<AQDynamicNPCController> DynamicNPCController = Cast<AQDynamicNPCController>(this->GetController());
	DynamicNPCController->MulticastShowSpeechBubbleWithText(FirstResponse.ResponseText);
	
	// @todo 2. Reply N2N 대사 AI에게 요청
	DynamicNPCController = Cast<AQDynamicNPCController>(TargetNPC->GetController());
	FOpenAIRequest Request(
		TargetNPC->FindComponentByClass<UNPCComponent>()->GetNPCID(),
		this->FindComponentByClass<UNPCComponent>()->GetNPCID(),
		EConversationType::N2N,
		FirstResponse.ResponseText
	);
	TargetNPC->FindComponentByClass<UNPCComponent>()->GetNPCResponse(DynamicNPCController, Request);
}

void AQDynamicNPC::ReplyConversationN2N(AQDynamicNPC* TargetNPC, FOpenAIResponse ReplyResponse)
{
	if (!HasAuthority())
	{
		return;
	}
	// 1.플레이어 시점 처리 (multicast)
	TObjectPtr<AQDynamicNPCController> DynamicNPCController;
	DynamicNPCController = Cast<AQDynamicNPCController>(this->GetController());
	DynamicNPCController->MulticastShowSpeechBubbleWithText(ReplyResponse.ResponseText);

	// 2. 대화 끝내기
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, TargetNPC]()
	{
		this->FinishConversationN2N();
		TargetNPC->FinishConversationN2N();
	});
	GetWorldTimerManager().SetTimer(N2NTimerHandle, TimerDel, 6.0f, false);
}

void AQDynamicNPC::FinishConversationN2N()
{
	if (!HasAuthority())
	{
		return;
	}
	
	// NPC UnFreeze & 대화창 끄기
	TObjectPtr<AQDynamicNPCController> DynamicNPCController;
	DynamicNPCController = Cast<AQDynamicNPCController>(this->GetController());
	DynamicNPCController->UnFreezePawn();
	DynamicNPCController->MulticastTurnOffSpeechBubble();
}

// ---------------------------------------------------------------------------------- //

void AQDynamicNPC::CountDownNMonologue()
{
	GetWorldTimerManager().SetTimer(NMonoTimerHandle, this, &AQDynamicNPC::RequestNMonologueText, 1.0f, false);
}

void AQDynamicNPC::RequestNMonologueText()
{
	if (!HasAuthority())
	{
		return;
	}
	AQDynamicNPCController* NPCController = Cast<AQDynamicNPCController>(GetController());
	// AI에게 혼잣말 대사 요청
	FOpenAIRequest Request(
	this->FindComponentByClass<UNPCComponent>()->GetNPCID(),
	this->FindComponentByClass<UNPCComponent>()->GetNPCID(),
	EConversationType::NMonologue,
	""
	);
	this->FindComponentByClass<UNPCComponent>()->GetNPCResponse(NPCController, Request);
}

void AQDynamicNPC::StartNMonologue(FOpenAIResponse Monologue)
{
	if (!HasAuthority())
	{
		return;
	}

	// 플레이어 시점 처리 (multicast)
	TObjectPtr<AQDynamicNPCController> DynamicNPCController = Cast<AQDynamicNPCController>(this->GetController());
	DynamicNPCController->MulticastShowSpeechBubbleWithText(Monologue.ResponseText);

	// 6초 뒤에 Finish
	GetWorldTimerManager().SetTimer(NMonoTimerHandle, this, &AQDynamicNPC::FinishNMonologue, 6.0f, false);
}

void AQDynamicNPC::FinishNMonologue()
{
	if (!HasAuthority())
	{
		return;
	}

	// 플레이어 시점 처리 (multicast)
	TObjectPtr<AQDynamicNPCController> DynamicNPCController = Cast<AQDynamicNPCController>(this->GetController());
	DynamicNPCController->MulticastTurnOffSpeechBubble();

	// NMonoCoolTime 초기화
	NMonoCoolTime = NMonoCoolTimeInit;
}

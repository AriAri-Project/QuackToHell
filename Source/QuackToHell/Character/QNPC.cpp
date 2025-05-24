// Copyright_Team_AriAri


#include "Character/QNPC.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "QLogCategories.h"
#include "UI/QPlayer2NSpeechBubbleWidget.h"
#include "Components/SphereComponent.h"
#include "NPCComponent.h"
#include "UI/QSpeechBubbleWidget.h"
#include "Net/UnrealNetwork.h"
#include "NPC/QDynamicNPCController.h"
#include "UI/QNameWidget.h"
#include "QLogCategories.h"

AQNPC::AQNPC(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{	/*위치값 세팅*/
	this->GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -50.f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	/*캡슐 콜라이더 세팅*/
	this->GetCapsuleComponent()->InitCapsuleSize(50.0f, 60.0f);
	/*NPC 컴포넌트*/
	NPCComponent = CreateDefaultSubobject<UNPCComponent>(TEXT("NPCComponent"));
	/*말풍선 UI 컴포넌트*/
	this->SpeechBubbleWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SpeechBubbleWidget"));
	EWidgetSpace WidgetSpace = EWidgetSpace::Screen;
	this->SpeechBubbleWidgetComponent->SetWidgetSpace(WidgetSpace);
	this->SpeechBubbleWidgetComponent->SetDrawAtDesiredSize(true);
	this->SpeechBubbleWidgetComponent->SetupAttachment(RootComponent);
	// **위젯 클래스는 블루프린트 에디터에서**:
	// 해당 컴포넌트의 Details → Widget Class 에 WBP_QSpeechBubble을 드래그해서 지정.
	// 언리얼이 클라이언트에서 알아서 BP 위젯을 생성해 줍니다.

	/*충돌처리*/
	InteractionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphereComponent->SetupAttachment(RootComponent);
	InteractionSphereComponent->SetSphereRadius(SphereRadius);

	/*충돌처리 바인딩*/
	InteractionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AQNPC::OnOverlapBegin);
	InteractionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &AQNPC::OnOverlapEnd);
}

void AQNPC::CountDownN2N()
{
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this]()
	{
		N2NCoolTimeCharged = true;
	});
	GetWorldTimerManager().SetTimer(N2NTimerHandle, TimerDel, 1.0f, false);
}

bool AQNPC::CheckCanStartConversN2N()
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

bool AQNPC::CheckCanFinishConversN2N()
{
	if (!HasAuthority())
	{
		false;
	}
	return true;
}

void AQNPC::RequestConversationN2N(AQNPC* TargetNPC)
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


	// 두 NPC가 모두 N2N 대화가 가능한 지 check
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
		this->FindComponentByClass<UNPCComponent>()->GetNPCResponse(this, Request);
	}
	else
	{
		UE_LOG(LogLogic, Log, TEXT("AQNPC::RequestConversationN2N - NPC can't start N2N Conversation."));
	}
}

void AQNPC::StartConversationN2N(AQNPC* TargetNPC, FOpenAIResponse FirstResponse)
{
	if (!HasAuthority())
	{
		return;
	}
	// 1. 플레이어 시점 말풍선 처리
	TObjectPtr<AQDynamicNPCController> DynamicNPCController = Cast<AQDynamicNPCController>(this->GetController());
	DynamicNPCController->MulticastShowSpeechBubbleWithText(FirstResponse.ResponseText);

	// @todo 2. Reply N2N 대사 AI에게 요청
	FOpenAIRequest Request(
		TargetNPC->FindComponentByClass<UNPCComponent>()->GetNPCID(),
		this->FindComponentByClass<UNPCComponent>()->GetNPCID(),
		EConversationType::N2N,
		FirstResponse.ResponseText
	);
	TargetNPC->FindComponentByClass<UNPCComponent>()->GetNPCResponse(TargetNPC, Request);
}

void AQNPC::ReplyConversationN2N(AQNPC* TargetNPC, FOpenAIResponse ReplyResponse)
{
	if (!HasAuthority())
	{
		return;
	}
	// 1.플레이어 시점 처리 (multicast)
	TObjectPtr<AQDynamicNPCController> DynamicNPCController;
		// 1-1. this npc 대사 띄우기
	DynamicNPCController = Cast<AQDynamicNPCController>(this->GetController());
	DynamicNPCController->MulticastShowSpeechBubbleWithText(ReplyResponse.ResponseText);
		// 1-2. 상대 npc 말풍선 끄기
	DynamicNPCController = Cast<AQDynamicNPCController>(TargetNPC->GetController());
	DynamicNPCController->MulticastTurnOffSpeechBubble();

	// 2. 대화 끝내기
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, TargetNPC]()
	{
		if (IsValid(this)) this->FinishConversationN2N();
		if (IsValid(TargetNPC)) TargetNPC->FinishConversationN2N();
	});
	GetWorldTimerManager().SetTimer(N2NTimerHandle, TimerDel, 6.0f, false);
}

void AQNPC::FinishConversationN2N()
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

void AQNPC::CountDownNMonologue()
{
	GetWorldTimerManager().SetTimer(NMonoTimerHandle, this, &AQNPC::RequestNMonologueText, 1.0f, false);
}

void AQNPC::RequestNMonologueText()
{
	if (!HasAuthority())
	{
		return;
	}
	
	// AI에게 혼잣말 대사 요청
	FOpenAIRequest Request(
	this->FindComponentByClass<UNPCComponent>()->GetNPCID(),
	this->FindComponentByClass<UNPCComponent>()->GetNPCID(),
	EConversationType::NMonologue,
	""
	);
	this->FindComponentByClass<UNPCComponent>()->GetNPCResponse(this, Request);
}

void AQNPC::StartNMonologue(FOpenAIResponse Monologue)
{
	if (!HasAuthority())
	{
		return;
	}

	// 플레이어 시점 처리 (multicast)
	TObjectPtr<AQDynamicNPCController> DynamicNPCController = Cast<AQDynamicNPCController>(this->GetController());
	DynamicNPCController->MulticastShowSpeechBubbleWithText(Monologue.ResponseText);

	// 6초 뒤에 Finish
	GetWorldTimerManager().SetTimer(NMonoTimerHandle, this, &AQNPC::FinishNMonologue, 6.0f, false);
}

void AQNPC::FinishNMonologue()
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


// ---------------------------------------------------------------------------------- //



void AQNPC::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// replicate할 프로퍼티 등록
	DOREPLIFETIME(AQNPC, bCanStartConversN2N);
	DOREPLIFETIME(AQNPC, bCanFinishConversN2N);
	DOREPLIFETIME(AQNPC, NPCConversationState);
}

void AQNPC::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AQNPC::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AQNPC::ServerRPCCanCanStartConversN2N_Implementation(const AQNPC* NPC)
{
}

void AQNPC::ServerRPCCanCanFinishConversN2N_Implementation(const AQNPC* NPC)
{
}


// ---------------------------------------------------------------------------------- //

UQSpeechBubbleWidget* AQNPC::GetSpeechBubbleWidget() const
{
	/**
	 * @todo SpeechBubbleWidget이 null이 아니게가져오기.
	 */
	if (SpeechBubbleWidget) {
		return SpeechBubbleWidget;
	}
	else {
		UE_LOG(LogLogic, Error, TEXT("AQNPC::GetSpeechBubbleWidget - SpeechBubbleWidget is nullptr"));
		return nullptr;
	}

}

// ---------------------------------------------------------------------------------- //

void AQNPC::BeginPlay()
{
	Super::BeginPlay();
	/*이름 세팅*/
	FString _Name = (NPCComponent && !NPCComponent->GetNPCName().IsEmpty())
		? NPCComponent->GetNPCName()
		: TEXT("이름비었음");
	this->SetCharacterName(_Name);
	Super::GetNameWidget()->SetNameWidgetText(GetCharacterName());
	/*말풍선 위젯 변수에 객체값 할당*/
	if (SpeechBubbleWidgetComponent)
	{
		UUserWidget* UserWidget = SpeechBubbleWidgetComponent->GetWidget();
		if (UserWidget)
		{
			SpeechBubbleWidget = Cast<UQSpeechBubbleWidget>(UserWidget);
		}
	}
}
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




bool AQNPC::GetCanStartConversN2N(const AQNPC* NPC)
{
	ServerRPCCanCanStartConversN2N_Implementation(NPC);
	return bCanStartConversN2N;
}

bool AQNPC::GetCanFinishConversN2N(const AQNPC* NPC)
{
	ServerRPCCanCanFinishConversN2N_Implementation(NPC);
	return bCanFinishConversN2N;
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


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
#include "UI/QNameWidget.h"

AQNPC::AQNPC()
{	/*위치값 세팅*/
	this->GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -50.f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
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
	TSubclassOf<UQSpeechBubbleWidget> _SpeechBubbleWidget;
	//UQSpeechBubbleWidget을 상속한 클래스만 담을 수 있도록 강제한다.
	this->SpeechBubbleWidgetComponent->SetWidgetClass(_SpeechBubbleWidget);

	/*허공말풍선 UI 컴포넌트*/
	this->Player2NSpeechBubbleWidgetComponent= CreateDefaultSubobject<UWidgetComponent>(TEXT("Player2NSpeechBubbleWidget"));
	this->Player2NSpeechBubbleWidgetComponent->SetWidgetSpace(WidgetSpace);
	this->Player2NSpeechBubbleWidgetComponent->SetDrawAtDesiredSize(true);
	this->Player2NSpeechBubbleWidgetComponent->SetupAttachment(RootComponent);
	TSubclassOf<UQPlayer2NSpeechBubbleWidget> _Player2NSpeechBubbleWidget;
	//UQPlayer2NSpeechBubbleWidget을 상속한 클래스만 담을 수 있도록 강제한다.
	this->Player2NSpeechBubbleWidgetComponent->SetWidgetClass(_Player2NSpeechBubbleWidget);

	/*충돌처리*/
	InteractionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphereComponent->SetupAttachment(RootComponent);
	InteractionSphereComponent->SetSphereRadius(SphereRadius);
	InteractionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AQNPC::OnOverlapBegin);
	InteractionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &AQNPC::OnOverlapEnd);
}

// ---------------------------------------------------------------------------------- //

void AQNPC::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//캐스팅 미성공시 nullptr
	TObjectPtr<AQNPC> OpponentNPC = Cast<AQNPC>(OtherActor);
	if (OpponentNPC) {
		OverlappingNPCs.Add(OtherActor);
	}
}

void AQNPC::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//캐스팅 미성공시 nullptr
	TObjectPtr<AQNPC> OpponentNPC = Cast<AQNPC>(OtherActor);
	if (OpponentNPC) {
		OverlappingNPCs.Remove(OtherActor);
	}
}

void AQNPC::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// replicate할 프로퍼티 등록
	DOREPLIFETIME(AQNPC, bCanStartConversN2N);
	DOREPLIFETIME(AQNPC, bCanFinishConversN2N);
}

void AQNPC::ServerRPCCanCanStartConversN2N_Implementation(const AQNPC* NPC)
{
}

void AQNPC::ServerRPCCanCanFinishConversN2N_Implementation(const AQNPC* NPC)
{
}




// ---------------------------------------------------------------------------------- //

TObjectPtr<class UQSpeechBubbleWidget> AQNPC::GetSpeechBubbleWidget() const
{
	return SpeechBubbleWidget;
}

TObjectPtr<class UQPlayer2NSpeechBubbleWidget> AQNPC::GetPlayer2NSpeechBubbleWidget() const
{
	return Player2NSpeechBubbleWidget;
}

TObjectPtr<AActor> AQNPC::GetClosestNPC()
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
	FString _Name = NPCComponent->GetNPCName();
	this->SetCharacterName(_Name);
	Super::GetNameWidget()->SetNameWidgetText(GetCharacterName());
	/*말풍선 위젯 변수에 객체값 할당*/
	if (SpeechBubbleWidget)
	{
		UUserWidget* UserWidget = SpeechBubbleWidgetComponent->GetWidget();
		if (UserWidget)
		{
			SpeechBubbleWidget = Cast<UQSpeechBubbleWidget>(UserWidget);
		}
	}
	/*말풍선 위젯 기본적으로 끈 채로 시작*/
	//SpeechBubbleWidget->TurnOffSpeechBubble();
	
	/*Player2N말풍선 위젯 변수에 객체값 할당*/
	if (Player2NSpeechBubbleWidget)
	{
		UUserWidget* UserWidget = Player2NSpeechBubbleWidgetComponent->GetWidget();
		if (UserWidget)
		{
			Player2NSpeechBubbleWidget = Cast<UQPlayer2NSpeechBubbleWidget>(UserWidget);
		}
	}
	/*Player2N말풍선 위젯 기본적으로 끈 채로 시작*/
	//Player2NSpeechBubbleWidget->TurnOffSpeechBubble();
}


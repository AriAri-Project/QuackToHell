// Copyright_Team_AriAri


#include "Character/QPlayer.h"
#include "Camera/CameraComponent.h"
#include "QLogCategories.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UI/QNameWidget.h"
#include "Character/QNPC.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/QPlayer2NSpeechBubbleWidget.h"
#include "Net/UnrealNetwork.h"


TObjectPtr<class UQPlayer2NSpeechBubbleWidget> AQPlayer::GetPlayer2NSpeechBubbleWidget() const
{
	return Player2NSpeechBubbleWidget;
}

AQPlayer::AQPlayer()
	:Super()
{
	
	/*캡슐 콜라이더 산하 컴포넌트*/
	RootComponent= this->GetCapsuleComponent();
	//충돌처리
	InteractionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphereComponent->SetupAttachment(RootComponent);
	InteractionSphereComponent->SetSphereRadius(SphereRadius);
	InteractionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AQPlayer::OnOverlapBegin);
	InteractionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &AQPlayer::OnOverlapEnd);
	//스프링암
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 460.f;
	

	/*스프링암 산하 컴포넌트*/
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	/*회전값 세팅*/
	this->bUseControllerRotationYaw = false;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritRoll = false;
	this->GetCharacterMovement()->bOrientRotationToMovement=true;
	/*위치값 세팅*/
	this->GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -18.f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
	/*캡슐 콜라이더 세팅*/
	this->GetCapsuleComponent()->InitCapsuleSize(21.0f, 21.0f);

	/*허공말풍선 UI 컴포넌트*/
	this->Player2NSpeechBubbleWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Player2NSpeechBubbleWidget"));
	EWidgetSpace WidgetSpace = EWidgetSpace::Screen;
	this->Player2NSpeechBubbleWidgetComponent->SetWidgetSpace(WidgetSpace);
	this->Player2NSpeechBubbleWidgetComponent->SetDrawAtDesiredSize(true);
	this->Player2NSpeechBubbleWidgetComponent->SetupAttachment(RootComponent);
	TSubclassOf<UQPlayer2NSpeechBubbleWidget> _Player2NSpeechBubbleWidget;
	//UQPlayer2NSpeechBubbleWidget을 상속한 클래스만 담을 수 있도록 강제한다.
	this->Player2NSpeechBubbleWidgetComponent->SetWidgetClass(_Player2NSpeechBubbleWidget);
}

TObjectPtr<AActor> AQPlayer::GetClosestNPC()
{
	if (OverlappingNPCs.Num() == 0) {
		//대화 대상 없음
		return nullptr;
	}

	TObjectPtr<AActor> ClosestNPC = nullptr;
	float MinDistance = FLT_MAX;

	for (TObjectPtr<AActor> NPC : OverlappingNPCs) {
		//플레이어와 NPC간 거리
		float Distance = FVector::Dist(this->GetActorLocation(), NPC->GetActorLocation());
		//최소거리찾기
		if (Distance < MinDistance) {
			MinDistance = Distance;
			ClosestNPC = NPC;
		}
	}

	return ClosestNPC;
}

void AQPlayer::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogLogic, Log, TEXT("Player의Beginplay호출"));
	/*이름 세팅*/
	FString _Name = TEXT("플레이어");
	this->SetCharacterName(_Name);
	Super::GetNameWidget()->SetNameWidgetText(GetCharacterName());


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



void AQPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//캐스팅 미성공시 nullptr
	TObjectPtr<AQNPC> OpponentNPC = Cast<AQNPC>(OtherActor);
	if (OpponentNPC) {
		OverlappingNPCs.Add(OtherActor);
	}
}

void AQPlayer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//캐스팅 미성공시 nullptr
	TObjectPtr<AQNPC> OpponentNPC = Cast<AQNPC>(OtherActor);
	if (OpponentNPC) {
		OverlappingNPCs.Remove(OtherActor);
	}
}




// -------------------------------------------------------------------------------------------------------- //
void AQPlayer::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// replicate할 프로퍼티 등록
	DOREPLIFETIME(AQPlayer, bCanStartConversP2N);
	DOREPLIFETIME(AQPlayer, bCanFinishConversP2N);
}

void AQPlayer::ServerRPCCanStartConversP2N_Implementation(const AQNPC* NPC)
{

}

void AQPlayer::ServerRPCCanFinishConversP2N_Implementation(const AQNPC* NPC)
{
}

void AQPlayer::ServerRPCStartConversation_Implementation(const AQNPC* NPC)
{
}

void AQPlayer::ServerRPCFinishConversation_Implementation(const AQNPC* NPC)
{
}

// ------------------------------------------------------------------------------------------------- //

bool AQPlayer::GetCanStartConversP2N(const AQNPC* NPC)
{
	ServerRPCCanStartConversP2N_Implementation(NPC);
	return bCanStartConversP2N;
}

bool AQPlayer::GetCanFinishConversP2N(const AQNPC* NPC)
{
	ServerRPCCanFinishConversP2N_Implementation(NPC);
	return bCanFinishConversP2N;
}

void AQPlayer::StartConversation(const AQNPC* NPC)
{
}

void AQPlayer::FinishConversation(const AQNPC* NPC)
{
}

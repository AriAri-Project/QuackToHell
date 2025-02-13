// Copyright_Team_AriAri


#include "Character/QPlayer.h"

#include "NPCComponent.h"
#include "Camera/CameraComponent.h"
#include "QLogCategories.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/QDynamicNPC.h"
#include "UI/QVillageUIManager.h"
#include "UI/QP2NWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UI/QNameWidget.h"
#include "Character/QNPC.h"
#include "Components/WidgetComponent.h"
#include "Player/QPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/QPlayer2NSpeechBubbleWidget.h"
#include "Net/UnrealNetwork.h"
#include "Player/QPlayerState.h"


TObjectPtr<class UQPlayer2NSpeechBubbleWidget> AQPlayer::GetPlayer2NSpeechBubbleWidget() const
{
	return Player2NSpeechBubbleWidget;
}

AQPlayer::AQPlayer(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
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
	SpringArmComponent->TargetArmLength = 660.f;
	FRotator SpringArmRotation = FRotator(0.f, 45.f, 0.f);
	SpringArmComponent->SetWorldRotation(SpringArmRotation);
	

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
	this->GetCapsuleComponent()->InitCapsuleSize(42.0f, 42.0f);

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
	if (LocalPlayerState)
	{
		FString _Name = LocalPlayerState->GetPlayerName();
		this->SetCharacterName(_Name);
		Super::GetNameWidget()->SetNameWidgetText(GetCharacterName());
	}
	
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

void AQPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	LocalPlayerState = NewController->GetPlayerState<AQPlayerState>();
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

}

void AQPlayer::ServerRPCCanStartConversP2N_Implementation(AQPlayerController* TargetController,  AQNPC* NPC)
{
    bool bResult = true;
	TObjectPtr<UNPCComponent> NPCComponent = NPC->FindComponentByClass<UNPCComponent>();

	if (LocalPlayerState == nullptr || NPCComponent == nullptr)
	{
		UE_LOG(LogLogic, Log, TEXT("ServerRPCCanStartConversP2N_Implementation -> LocalPlayerState or NPCComponent is null."));
		bResult = false;
	}
	
	// OpenAI에 Request를 보낼 수 있는지 확인
	if (!NPCComponent->CanSendOpenAIRequest())
	{
		UE_LOG(LogLogic, Log, TEXT("ServerRPCCanStartConversP2N_Implementation -> CanSendOpenAIRequest is false"));
		bResult = false;
	}
	// Player ConversationState가 None인지 확인
	else if (LocalPlayerState->GetPlayerConversationState() != EConversationType::None)
	{
		UE_LOG(LogLogic, Log, TEXT("ServerRPCCanStartConversP2N_Implementation -> Player ConversationState is not None but %s"), *UEnum::GetValueAsString(LocalPlayerState->GetPlayerConversationState()) );
		bResult = false;
	}
	// NPC ConversationState가 None인지 확인
	else if (NPC->GetNPCConversationState() != EConversationType::None)
	{
		UE_LOG(LogLogic, Log, TEXT("ServerRPCCanStartConversP2N_Implementation -> NPC ConversationState is not None"));
		bResult = false;
	}
	// NPC와 이전에 대화한 적 있는지 확인
	else if (!NPCComponent->GetIsFirstConversation())
	{
		UE_LOG(LogLogic, Log, TEXT("ServerRPCCanStartConversP2N_Implementation -> This is not a first conversation."));
		bResult = false;
	}
	TargetController->ClientRPCUpdateCanStartConversP2N(bResult);
}


void AQPlayer::ServerRPCCanFinishConversP2N_Implementation(AQPlayerController* TargetController, AQNPC* NPC)
{
	bool bResult = true;
	TObjectPtr<UNPCComponent> NPCComponent = NPC->FindComponentByClass<UNPCComponent>();

	// Player ConversationState가 None인지 확인
	if (LocalPlayerState->GetPlayerConversationState() == EConversationType::None)
	{
		bResult = false;
	}
	
	// NPC ConversationState가 None인지 확인
	else if (NPC->GetNPCConversationState() == EConversationType::None)
	{
		bResult = false;
	}

	// OpenAI로부터 기다리고 있는 답변이 있는지
	else if (NPCComponent->GetIsRequestInProgress())
	{
		bResult = false;
	}
	
	TargetController->ClientRPCUpdateCanFinishConversP2N(bResult);
}


void AQPlayer::ServerRPCStartConversation_Implementation(AQNPC* NPC)
{
	// 상태 업데이트
	LocalPlayerState->SetPlayerConverstationState(EConversationType::P2N);
	NPC->SetNPCConversationState(EConversationType::P2N);
	
	// 다른 플레이어들 시점 처리
	AQPlayer* LocalPlayer = Cast<AQPlayer>(LocalPlayerState->GetPawn());
	if (LocalPlayer)
	{
		MulticastRPCStartConversation(LocalPlayer, NPC);
	}

	// OpenAI에게 NPC의 첫 대사 요청하기
	FString Temp = TEXT("");
	FOpenAIRequest Request(
		LocalPlayerState->GetPlayerId(),
		NPC->FindComponentByClass<UNPCComponent>()->GetNPCID(),
		EConversationType::P2N,
		Temp
	);
	NPC->FindComponentByClass<UNPCComponent>()->ServerRPCGetNPCResponse(Request);
}



void AQPlayer::MulticastRPCStartConversation_Implementation(AQPlayer* Player, AQNPC* NPC)
{
	if (LocalPlayerState == nullptr)
	{
		UE_LOG(LogLogic, Log, TEXT("AQPlayer::MulticastRPCStartConversation_Implementation -> LocalPlayerState is null."));
		return;
	}
	AQPlayer* LocalPlayer = Cast<AQPlayer>(LocalPlayerState->GetPawn());
	if (HasAuthority() || LocalPlayer == Player)
	{
		return;
	}
	/** @todo 유진 Player2 시점에서 Player1과 NPC 머리위에 공백 말풍선 띄우기 */
	Player->GetPlayer2NSpeechBubbleWidget()->TurnOnSpeechBubble();
	Cast<AQDynamicNPC>(NPC)->GetPlayer2NSpeechBubbleWidget()->TurnOnSpeechBubble();
}

void AQPlayer::ServerRPCFinishConversation_Implementation(AQPlayerController* TargetController,  AQNPC* NPC)
{
	bool bResult = false;
	// 상태 업데이트
	if (LocalPlayerState == nullptr || NPC == nullptr)
	{
		return;
	}
	LocalPlayerState->SetPlayerConverstationState(EConversationType::None);
	NPC->SetNPCConversationState(EConversationType::None);


	// 다른 플레이어들 시점 처리
	AQPlayer* LocalPlayer = Cast<AQPlayer>(LocalPlayerState->GetPawn());
	if (LocalPlayer)
	{
		MulticastRPCFinishConversation(LocalPlayer, NPC);
	}

	//클라이언트에게 대화끝내기 처리 요청
	TargetController->ClientRPCFinishConversation(NPC);
}



void AQPlayer::MulticastRPCFinishConversation_Implementation(AQPlayer* Player, AQNPC* NPC)
{
	if (HasAuthority() || LocalPlayerState->GetPawn() == Player)
	{
		return;
	}
	/** @todo 유진 Player2 시점에서 Player1과 NPC 머리위에 공백 말풍선 제거*/
	Player->GetPlayer2NSpeechBubbleWidget()->TurnOffSpeechBubble();
	Cast<AQDynamicNPC>(NPC)->GetPlayer2NSpeechBubbleWidget()->TurnOffSpeechBubble();
}



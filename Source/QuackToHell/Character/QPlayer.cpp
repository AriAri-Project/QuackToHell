// Copyright_Team_AriAri


#include "Character/QPlayer.h"

#include "NPCComponent.h"
#include "Camera/CameraComponent.h"
#include "QLogCategories.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/QDynamicNPC.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UI/QNameWidget.h"
#include "Character/QNPC.h"
#include "Components/WidgetComponent.h"
#include "Player/QPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/QPlayer2NSpeechBubbleWidget.h"
#include "Net/UnrealNetwork.h"
#include "NPC/QDynamicNPCController.h"
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
	Player2NSpeechBubbleWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Player2NSpeechBubbleWidget"));
	Player2NSpeechBubbleWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	Player2NSpeechBubbleWidgetComponent->SetDrawAtDesiredSize(true);
	Player2NSpeechBubbleWidgetComponent->SetupAttachment(RootComponent);
	Player2NSpeechBubbleWidgetComponent->SetVisibility(true);
	Player2NSpeechBubbleWidgetComponent->SetHiddenInGame(false);
	TSubclassOf<UQPlayer2NSpeechBubbleWidget> _Player2NSpeechBubbleWidget;
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
	
	UE_LOG(LogLogic, Log, TEXT("AQPlayer::BeginPlay started - Name: %s, NetMode: %d"), *this->GetName(), this->GetNetMode());

	// 캐릭터 이름 UI에 플레이어 이름 띄우기
	GetNameWidget()->SetNameWidgetText(GetCharacterName());
	
	// Player2N 말풍선 위젯 변수에 객체값 할당
	if (Player2NSpeechBubbleWidgetComponent)
	{
		UUserWidget* UserWidget = Player2NSpeechBubbleWidgetComponent->GetWidget();
		if (UserWidget)
		{
			Player2NSpeechBubbleWidget = Cast<UQPlayer2NSpeechBubbleWidget>(UserWidget);
		}
	}
	// Player2N말풍선 위젯 기본적으로 끈 채로 시작
	Player2NSpeechBubbleWidget->TurnOffSpeechBubble();
}

void AQPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// 클라이언트 환경 : 플레이어 이름 데이터 가져오기
	ThisPlayerState = GetPlayerState<AQPlayerState>();
	if (ThisPlayerState)
	{
		FString Name = ThisPlayerState->GetPlayerName();
		UE_LOG(LogLogic, Log, TEXT("AQPlayer::OnRep_PlayerState- playerName : %s, NetMode: %d"), *Name, this->GetNetMode());
		SetCharacterName(Name);
		GetNameWidget()->SetNameWidgetText(Name);
	}
}

void AQPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 호스트 환경 : 플레이어 이름 데이터 가져오기 
	MyPlayerController = Cast<AQPlayerController>(NewController);
	if (MyPlayerController)
	{
		ThisPlayerState = MyPlayerController->GetPlayerState<AQPlayerState>();
		if (ThisPlayerState)
		{
			FString _Name = ThisPlayerState->GetPlayerName();
			UE_LOG(LogLogic, Log, TEXT("AQPlayer::PossessedBy - playerName : %s, NewController : %s, NetMode: %d"), *_Name, *NewController->GetName(), this->GetNetMode());
			this->SetCharacterName(_Name);
			if (GetNameWidget() != nullptr)
			{
				GetNameWidget()->SetNameWidgetText(_Name);
			}
		}
	}
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

	if (ThisPlayerState == nullptr || NPCComponent == nullptr)
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
	else if (ThisPlayerState->GetPlayerConversationState() != EConversationType::None)
	{
		UE_LOG(LogLogic, Log, TEXT("ServerRPCCanStartConversP2N_Implementation -> Player ConversationState is not None but %s"), *UEnum::GetValueAsString(ThisPlayerState->GetPlayerConversationState()) );
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
	if (ThisPlayerState->GetPlayerConversationState() == EConversationType::None)
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
	ThisPlayerState->SetPlayerConverstationState(EConversationType::P2N);
	NPC->SetNPCConversationState(EConversationType::P2N);

	// NPC Freeze
	TObjectPtr<AQDynamicNPCController> NPCController = Cast<AQDynamicNPCController>(NPC->GetController());
	if (NPCController)
	{
		NPCController->FreezePawn();
	}
	
	// 다른 플레이어들 시점 처리
	for (TActorIterator<AQPlayer> It(GetWorld()); It; ++It)
	{
		AQPlayer* Peer = *It;
		Peer->MulticastRPCStartConversation(this, NPC);
	}

	// OpenAI에게 NPC의 첫 대사 요청하기
	FString Temp = TEXT("");
	FOpenAIRequest Request(
		NPC->FindComponentByClass<UNPCComponent>()->GetNPCID(),
		ThisPlayerState->GetPlayerId(),
		EConversationType::PStart,
		Temp
	);
	NPC->FindComponentByClass<UNPCComponent>()->ServerRPCGetNPCResponse(Request);
}

void AQPlayer::ServerRPCFinishConversation_Implementation(AQPlayerController* TargetController,  AQNPC* NPC)
{
	bool bResult = false;
	// 상태 업데이트
	if (ThisPlayerState == nullptr || NPC == nullptr)
	{
		return;
	}
	ThisPlayerState->SetPlayerConverstationState(EConversationType::None);
	NPC->SetNPCConversationState(EConversationType::None);
	
	// NPC Pawn UnFreeze
	TObjectPtr<AQDynamicNPCController> NPCController = Cast<AQDynamicNPCController>(NPC->GetController());
	if (NPCController)
	{
		NPCController->UnFreezePawn();
	}
	
	// 다른 플레이어들 시점 처리
	for (TActorIterator<AQPlayer> It(GetWorld()); It; ++It)
	{
		AQPlayer* Peer = *It;
		Peer->MulticastRPCFinishConversation(this, NPC);
	}

	//클라이언트에게 대화끝내기 처리 요청
	TargetController->ClientRPCFinishConversation(NPC);
}

void AQPlayer::MulticastRPCStartConversation_Implementation(AQPlayer* Player, AQNPC* NPC)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AQPlayer* LocalPlayer = Cast<AQPlayer>(Pawn);
	if (Player == LocalPlayer)
	{
		return;
	}
	Player->GetPlayer2NSpeechBubbleWidget()->TurnOnSpeechBubble();
	TObjectPtr<AQDynamicNPC> DynamicNPC = Cast<AQDynamicNPC>(NPC);
	if (DynamicNPC)
	{
		DynamicNPC->GetPlayer2NSpeechBubbleWidget()->TurnOnSpeechBubble();
	}
}

void AQPlayer::MulticastRPCFinishConversation_Implementation(AQPlayer* Player, AQNPC* NPC)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AQPlayer* LocalPlayer = Cast<AQPlayer>(Pawn);
	if (Player == LocalPlayer)
	{
		return;
	}
	Player->GetPlayer2NSpeechBubbleWidget()->TurnOffSpeechBubble();
	TObjectPtr<AQDynamicNPC> DynamicNPC = Cast<AQDynamicNPC>(NPC);
	if (DynamicNPC)
	{
		DynamicNPC->GetPlayer2NSpeechBubbleWidget()->TurnOffSpeechBubble();
	}
}



// Copyright_Team_AriAri


#include "NPC/QDynamicNPCController.h"
#include "Character/QDynamicNPC.h"
#include "BehaviorTree/BehaviorTree.h"                // Behavior Tree 클래스
#include "BehaviorTree/BlackboardComponent.h"         // Blackboard 컴포넌트
#include "BehaviorTree/BehaviorTreeComponent.h"       // Behavior Tree 컴포넌트
#include "QLogCategories.h"
#include "Engine/World.h"
#include "NPCComponent.h"
#include "UI/QSpeechBubbleWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "UI/QP2NWidget.h"
#include "Character/QDynamicNPC.h"
#include "Character/QPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/QVillageUIManager.h"
#include "Character/QNPC.h"
#include "Kismet/GameplayStatics.h"
#include "Player/QPlayerController.h"
#include "Player/QPlayerState.h"
void AQDynamicNPCController::BeginPlay()
{
    Super::BeginPlay();
    // Behavior Tree와 Blackboard 컴포넌트가 유효한지 확인
    if (BehaviorTree && BehaviorTree->BlackboardAsset)
    {
        UE_LOG(LogLogic, Log, TEXT("run behavior"));
        // Blackboard 초기화
        UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComp);

        // Behavior Tree 실행
        RunBehaviorTree(BehaviorTree);
    }

    //NPCComponent 대입하기
    NPCComponent = Cast<AQDynamicNPC>(GetPawn())->FindComponentByClass<UNPCComponent>();
    //VillageManager 대입하기
    VillageUIManager = AQVillageUIManager::GetInstance(GetWorld());
    //mypawn대입하기
    MyPawn = Cast<AQNPC>(GetPawn());


}


void AQDynamicNPCController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //회전 업데이트 처리
    if (bIsRotating) {
        UpdateRotation();
    }

    //가장 가까운 NPC가 있다면 서버에게 대화가능한지 체크
    if (TObjectPtr<AQNPC> OpponentNPC = Cast<AQNPC>(Cast<AQDynamicNPC>(MyPawn)->GetClosestNPC())) {
        if (MyPawn->GetCanStartConversN2N(OpponentNPC)) {
            //@서버: 대화시작하라고 함수호출하기
            StartDialog(OpponentNPC, ENPCConversationType::N2N);
        }
    }
}

void AQDynamicNPCController::UpdateRotation()
{

    //현재 NPC 위치
    FVector NPCPosition = GetPawn()->GetActorLocation();

    //상대방의 위치
    FVector OpponentPosition = OpponentPawn->GetActorLocation();

    //목표 방향으로 회전 계산
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(NPCPosition, OpponentPosition);

    //현재 Rotation 가져오기
    FRotator CurrentRotation = GetPawn()->GetActorRotation();

    //부드러운 회전 적용 (보간)
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, GetWorld()->GetDeltaSeconds(), 5.0f);
    NewRotation = FRotator(CurrentRotation.Pitch, NewRotation.Yaw, CurrentRotation.Roll);

    //회전 적용
    GetPawn()->SetActorRotation(NewRotation);

    if (FMath::Abs(CurrentRotation.Yaw - LookAtRotation.Yaw) < 1.0f) {
        SetActorTickEnabled(false);
        bIsRotating = false;
        OpponentPawn = nullptr;
    }
}



void AQDynamicNPCController::MulticastShowSpeechBubbleWithText_Implementation(const FString& Text) const
{
    //MyPawn->GetSpeechBubbleWidget()->으로 updatetext, turnof/off 접근가능
    if (MyPawn) {
		MyPawn->GetSpeechBubbleWidget()->TurnOnSpeechBubble();
        MyPawn->GetSpeechBubbleWidget()->UpdateText(Text);
    }
    else {
        UE_LOG(LogLogic, Error, TEXT("AQDynamicNPCController::MulticastShowSpeechBubbleWithText_Implementation - MyPawn is nullptr"));
        return;
    }
}

void AQDynamicNPCController::MulticastTurnOffSpeechBubble_Implementation()
{
    //MyPawn->GetSpeechBubbleWidget()->으로 updatetext, turnof/off 접근가능
    if (MyPawn) {
		MyPawn->GetSpeechBubbleWidget()->TurnOffSpeechBubble();
    }
    else {
        UE_LOG(LogLogic, Error, TEXT("AQDynamicNPCController::MulticastTurnOffSpeechBubble_Implementation - MyPawn is nullptr"));
        return;
    }
}

void AQDynamicNPCController::UnFreezePawn()
{
    //폰 정보 가져오기
    TObjectPtr<APawn> ControlledPawn = this->GetPawn();
    if (!ControlledPawn) {
        return;
    }

    //movement component 가져오기
    TObjectPtr<UCharacterMovementComponent> MovementComponent = ControlledPawn->FindComponentByClass<UCharacterMovementComponent>();
    if (MovementComponent) {
        MovementComponent->SetComponentTickEnabled(true);//틱 활성화
        MovementComponent->Activate(true);//이동 활성화
    }

    UE_LOG(LogLogic, Log, TEXT("NPC 이동 재개."));
}

void AQDynamicNPCController::FreezePawn()
{
    UE_LOG(LogLogic, Log, TEXT(" QDynamicNPCController::AQDynamicNPCController - NPC FreezePawn start, NetMode: %d"), GetNetMode());
	
    //폰 정보 가져오기
    APawn* ControlledPawn = this->GetPawn();
    if (!ControlledPawn) {
        UE_LOG(LogLogic, Error, TEXT("FreezePawn: ControlledPawn is nullptr"));
        return;
    }

    //movement component 가져오기
    TObjectPtr<UCharacterMovementComponent> MovementComponent = ControlledPawn->FindComponentByClass<UCharacterMovementComponent>();
    if (MovementComponent) {
        MovementComponent->StopMovementImmediately();//즉시멈춤
        MovementComponent->SetComponentTickEnabled(false);//틱 비활성화
        MovementComponent->Deactivate();//이동 비활성화
    }
    else if (!MovementComponent) {
        UE_LOG(LogLogic, Error, TEXT("FreezePawn: MovementComponent is nullptr"));
        return;
    }

    UE_LOG(LogLogic, Log, TEXT("NPC 멈춤."));
}

void AQDynamicNPCController::RotateToOpponent(const TObjectPtr<APawn> InputOpponentPawn) {
    if (!InputOpponentPawn) {
        UE_LOG(LogLogic, Log, TEXT("AQNPCController: StartDialog에서 유효한 Pawn정보를 넘겨주지 않았습니다."));
        return;
    }

    //회전 업데이트 시작
    OpponentPawn = InputOpponentPawn;
    bIsRotating = true;
    SetActorTickEnabled(true);
}

void AQDynamicNPCController::StartDialog(TObjectPtr<APawn> _OpponentPawn, ENPCConversationType  ConversationType)
{
    //ConversationType에 따라 StartDialog다르게 처리
    switch (ConversationType)
    {
    case ENPCConversationType::P2N:
    {
        ////몸멈추기 & 상대방을 향해 회전하기
        FreezePawn();
        RotateToOpponent(_OpponentPawn);
    }
    break;
    case ENPCConversationType::N2N:
    {
        //몸멈추기 & 상대방을 향해 회전하기
        FreezePawn();
        RotateToOpponent(_OpponentPawn);
        /**
        * @todo 서버: 대화 시작 로직 전개하기 : 상태관리, 기타조건 .. 처리하기
        * //조건 : 처음 나타나는 텍스트는 “(상대 망자NPC 이름), 아까(가장 마지막으로 대화한	플레이어 닉네임)과 대화를 나누었습니다.”이다.
        * 더 자세한 조건 : 컨플루언스 확인하기 https://ariari-ewha.atlassian.net/wiki/spaces/~712020e9509d1767994750b2ca1d1e408ddb2d/pages/42893472
        */

        /* test code임. 되는 것 확인함 https://ariari-ewha.atlassian.net/browse/SCRUM-593?atlOrigin=eyJpIjoiNDY1NDI0ZGNjNTMwNDJjYjg5ODdiMmEzODQyM2IyM2MiLCJwIjoiaiJ9*/
        AQPlayerState* _PlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<AQPlayerState>();
        FString NPCName = NPCComponent->GetNPCName();
        FString PlayerName = _PlayerState->GetPlayerName();
        FString SpeechText = FString::Printf(TEXT("%s, 아까 %s과 대화를 나누었습니다."), *NPCName, *PlayerName);
        MulticastShowSpeechBubbleWithText(SpeechText);

        /**
        * .
        * @todo 서버: 대화 끝난 후, EndDialog호출해 UnFreezePawn()수행되게 한다.
        *              대화 끝난 후, MulticastTurnOffSpeechBubble() 호출해 말풍선 끈다.
        */
    }
    break;
    default:
        break;
    }
}


void AQDynamicNPCController::EndDialog()
{
    UnFreezePawn();
}

/*
void AQDynamicNPCController::Response(FString& Text, EConversationType InputConversationType)
{
    //정보가져오기
    FOpenAIRequest OpenAIRequest;
    OpenAIRequest.ConversationType = InputConversationType;
    AQPlayerController* PlayerController = Cast<AQPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    OpenAIRequest.ListenerID = Cast<AQPlayerState> (PlayerController->PlayerState)->GetPlayerId();
    OpenAIRequest.Prompt = Text;
    OpenAIRequest.SpeakerID = NPCComponent->GetNPCID();
    NPCComponent->ServerRPCGetNPCResponse(OpenAIRequest);
}
*/
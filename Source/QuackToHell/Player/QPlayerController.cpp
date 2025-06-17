// Copyright_Team_AriAri


#include "Player/QPlayerController.h"
#include "QLogCategories.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "UI/QVillageUIManager.h"
#include "UI/QMapWidget.h"
#include "Game/QVillageGameState.h"
#include "UI/QP2NWidget.h"
#include "Engine/Engine.h"  
#include "UI/QEvidenceWidget.h"
#include "EnhancedInputComponent.h"
#include "UI/QInventoryWidget.h"
#include "Character/QPlayer.h"
#include "Character/QDynamicNPC.h"
#include "NPC/QDynamicNPCController.h"
#include "Character/QNPC.h"
#include "Actors/AQEvidenceActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NPC/QNPCController.h"
#include "Player/QPlayerState.h"
#include "QGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "NPC/QDynamicNPCController.h"

void AQPlayerController::MulticastBlockInteraction_Implementation()
{
	UE_LOG(LogLogic, Log, TEXT("AQPlayerController::BlockInteraction: 구현됨. "));
	//상호작용을 block == 이동입력처리x
	//EnhancedInputComponent 캐스팅
	auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent) {
		//바인딩 해제
		EnhancedInputComponent->ClearActionBindings();
	}
	//InputComponent를 비활성화
	InputComponent = nullptr;
}

void AQPlayerController::ServerRPCStartMatchMaking_Implementation(AQPlayerController* ClientPC)
{
}

void AQPlayerController::ServerRPCGetProsecutorStatement_Implementation(AQPlayerController* ClientPC)
{
}

void AQPlayerController::ClientRPCGetProsecutorStatement_Implementation(FOpenAIResponse Response)
{
}

void AQPlayerController::ServerRPCGetLawyerStatement_Implementation(AQPlayerController* ClientPC)
{
}

void AQPlayerController::ClientRPCGetLawyerStatement_Implementation(FOpenAIResponse Response)
{
}

void AQPlayerController::ServerRPCGetFinalJudgement_Implementation(AQPlayerController* ClientPC)
{
}

void AQPlayerController::ClientRPCGetFinalJudgement_Implementation(FOpenAIResponse Response)
{
}

void AQPlayerController::ServerRPCTravelToMain_Implementation(AQPlayerController* ClientPC)
{
}

void AQPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetNetMode() != NM_DedicatedServer && !HasAuthority()) {
		UE_LOG(LogTemp, Log, TEXT("💡 Local Client PlayerController BeginPlay - VillageUIManager 생성 시도"));

		// VillageUIManager 싱글톤 생성
		AQVillageUIManager::GetInstance(GetWorld());
	}

	//Player로부터 EnhancedInput subsystem을 가져온다. 
	auto* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (SubSystem) {
		//mapping context를 추가해주는데, priority는 0
		SubSystem->AddMappingContext(InputMappingContext, 0);
	}

	//villagemanager를 가져온다
	VillageUIManager = AQVillageUIManager::GetInstance(GetWorld());

	//기본적으로 마우스커서 보이게
	this->bShowMouseCursor = true;
}

void AQPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//InputComponent는 Actor의 멤버변수이다. 멤버변수를 EnhancedInput으로 캐스팅한다.
	auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent) {
		/*액션에 대해, 트리거 되면 콜백되는 함수를 바인딩한다.*/
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ThisClass::InputTurn);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &ThisClass::InputInteraction);
		EnhancedInputComponent->BindAction(EnableTurnAction, ETriggerEvent::Started, this, &ThisClass::InputEnableTurn);
		EnhancedInputComponent->BindAction(TurnOnOffMapAction, ETriggerEvent::Started, this, &ThisClass::InputTurnOnOffMap);
		EnhancedInputComponent->BindAction(TurnOnOffInventoryAction, ETriggerEvent::Started, this, &ThisClass::InputTurnOnOffInventory);
		EnhancedInputComponent->BindAction(EnableTurnAction, ETriggerEvent::Completed, this, &ThisClass::InputEnableTurn);
	}
}

void AQPlayerController::Tick(float DeltaTime)
{
	if (InputComponent == nullptr) {
		MoveToCourtTimer += DeltaTime;
		if (MoveToCourtTimer >= MoveToCourtTimerMax) {
			// 현재 월드에서 GameState를 가져옴
			TObjectPtr<AQVillageGameState> VillageGameState = GetWorld()->GetGameState<AQVillageGameState>();
			if (!VillageGameState)
			{
				/** @todo 인수 형식맞게 변환하기. */
				//VillageGameState->ServerRPCRequestTravelToCourt(this, true);
			}
		}
	}
}

void AQPlayerController::ClientRPCUpdateCanFinishConversP2N_Implementation(bool bResult)
{
	if (bResult)
	{
		UE_LOG(LogLogic, Log, TEXT("b대화끝내기  = true"));
		TObjectPtr<AQPlayer> _Player = Cast<AQPlayer>(this->GetPawn());
		TObjectPtr<AQDynamicNPC> _NPC = Cast<AQDynamicNPC>(_Player->GetClosestNPC());
		Cast<AQPlayer>(this->GetPawn())->ServerRPCFinishConversation(this, _NPC);
	}
	else
	{
		/** @todo 유진 : 대화를 시작할 수 없을때 클라이언트에서 실행시켜야하는 함수 여기서 호출 */
		UE_LOG(LogLogic, Log, TEXT("대화끝내기실패!!"));

	}
}

void AQPlayerController::ClientRPCUpdateCanStartConversP2N_Implementation(bool bResult)
{
	if (!bResult)
	{
		UE_LOG(LogLogic, Log, TEXT("대화시도실패!!"));
		return;
	}
	TObjectPtr<AQPlayer> _Player = Cast<AQPlayer>(this->GetPawn());
	TObjectPtr<AQDynamicNPC> NPC = Cast<AQDynamicNPC>(_Player->GetClosestNPC());
	/*
	 *TObjectPtr<AQDynamicNPCController> DynamicNPCController = Cast<AQDynamicNPCController>(NPC->GetController());
	if (!DynamicNPCController) {
		UE_LOG(LogLogic, Error, TEXT("DynamicNPCController가 없음"));
		return;
	}
	*/
	/*몸 멈추기*/
	//내 몸멈추기
	FreezePawn();
	//NPC몸멈추기
	//DynamicNPCController->FreezePawn();
	
	//플레이어를 대화처리한다. (서버)
	Cast<AQPlayer>(this->GetPawn())->ServerRPCStartConversation(this, NPC);
}

void AQPlayerController::ClientRPCStartConversation_Implementation(FOpenAIResponse NPCStartResponse, AQNPC* NPC)
{
	// UE_LOG(LogTemp, Log, TEXT("Player Conversation State Updated. -> %hhd"), LocalPlayerState->GetPlayerConversationState());
	// UE_LOG(LogTemp, Log, TEXT("%s Conversation State Updated. -> %hhd"), *NPC->GetName(), NPC->GetNPCConversationState());

		UE_LOG(LogLogic, Log, TEXT("ClientRPCStartConversation Started."))
		//0. 상대방 NPC를 불러옴
		TObjectPtr<AQDynamicNPC> _NPC = Cast<AQDynamicNPC>(NPC);
		if (_NPC == nullptr)
		{
			UE_LOG(LogLogic, Error, TEXT("AQPlayerController::ClientRPCStartConversation_Implementation - _NPC is nullptr."))
			return;
		}

		/** @todo 유진 : 서버측에서 대화 시작 로직이 성공적으로 마무리 되었을 떄 실행할 함수 여기서 호출 */
		/** @todo 유진 - 이 부분을 Player.cpp에 표시해놓은 부분에서 호출하면 될듯. + nullcheck같은거 추가로 하면ㄷ 좋을 것 같아요*/
		//1. UI를 킨다.
		VillageUIManager->TurnOnUI(EVillageUIType::P2N);
	
		//2. P2N Widget에게 자신의 정보를 넘긴다.
		//내 정보 넘겨주기
		Cast<UQP2NWidget>((VillageUIManager->GetActivedWidget(EVillageUIType::P2N)))->SetConversingPlayer(this);
		//3. P2N Widget에게 NPC의 정보를 넘긴다.
		TObjectPtr<UQP2NWidget> P2NWidget = Cast<UQP2NWidget>(Cast<UQP2NWidget>(VillageUIManager->GetActivedWidget(EVillageUIType::P2N)));
		P2NWidget->SetConversingNPC(_NPC);
		P2NWidget->DisplayNPCResponse(NPCStartResponse);
}

void AQPlayerController::ClientRPCUpdateP2NResponse_Implementation(FOpenAIResponse Response)
{
	TObjectPtr<UQP2NWidget> P2NWidget = Cast<UQP2NWidget>(VillageUIManager->GetActivedWidget(EVillageUIType::P2N));
	P2NWidget->DisplayNPCResponse(Response);
}

void AQPlayerController::ClientRPCFinishConversation_Implementation(AQNPC* NPC)
{
	//UE_LOG(LogTemp, Log, TEXT("Player Conversation State Updated. -> %hhd"), LocalPlayerState->GetPlayerConversationState());
	UE_LOG(LogTemp, Log, TEXT("%s Conversation State Updated. -> %hhd"), *NPC->GetName(), NPC->GetNPCConversationState());

		/** @todo 유진 : 서버측에서 대화 마무리 로직이 성공적으로 마무리 되었을 때 실행할 함수 여기서 호출 */
		/** @todo 유진 : 대화를 끝낼 수 있을때 클라이언트에서 실행시켜야하는 함수 여기서 호출 */
		
		//몸 푼다.
		UnFreezePawn();
		//UI끈다.
		AQVillageUIManager::GetInstance(GetWorld())->TurnOffUI(EVillageUIType::P2N);
}


void AQPlayerController::FreezePawn()
{
	//폰 정보 가져오기
	TObjectPtr<APawn> ControlledPawn = this->GetPawn();
	if (!ControlledPawn) {
		return;
	}

	//movement component 가져오기
	TObjectPtr<UCharacterMovementComponent> MovementComponent = ControlledPawn->FindComponentByClass<UCharacterMovementComponent>();
	if (MovementComponent) {
		MovementComponent->StopMovementImmediately();//즉시멈춤(속도를 0으로 만듦)
		MovementComponent->SetComponentTickEnabled(false);//틱 비활성화
		MovementComponent->Deactivate();//이동 비활성화
	}

	UE_LOG(LogLogic, Log, TEXT("플레이어 멈춤."));
}

void AQPlayerController::UnFreezePawn()
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

	UE_LOG(LogLogic, Log, TEXT("플레이어 이동 재개."));
}

void AQPlayerController::GoToCourt()
{
	AQVillageGameState* VillageGameState = GetWorld()->GetGameState<AQVillageGameState>();
	if (VillageGameState)
	{
		AQPlayerState* _PlayerState = GetPlayerState<AQPlayerState>();
		// 재판장으로 이동 요청
		VillageGameState->ServerRPCRequestTravelToCourt(_PlayerState, true);
	}
	else
	{
		UE_LOG(LogLogic, Error, TEXT("AQPlayerController::GoToCourt - VillageGameState is nullptr."));
	}
}




void AQPlayerController::InputEnableTurn(const FInputActionValue& InputValue)
{
	bEnableTurn = InputValue.Get<bool>() ? true : false;
	//마우스 커서 감추기
	this->bShowMouseCursor = InputValue.Get<bool>() ? false : true;
}

void AQPlayerController::InputInteraction(const FInputActionValue& InputValue)
{
	TObjectPtr<AQPlayer> _Player = Cast<AQPlayer>(this->GetPawn());
	
	/* 증거물 줍기 */
	AAQEvidenceActor* Evidence = Cast<AAQEvidenceActor>(_Player->GetClosestEvidence());
	//증거물데이터 로그에 찍어보기
	if (Evidence)
	{
		// 값 복사
		FEvidence Data = Evidence->GetEvidenceData();
		FString Name = Data.GetName();
		FString Description = Data.GetDescription();
		FString Message = FString::Printf(TEXT("Picked up evidence: %s\n%s"), *Name, *Description);


		// ① 화면에 초록색으로 5초 동안 메시지 띄우기
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				/* Key */    -1,           // -1 이면 매번 새 메시지
				/* Time */    5.0f,         // 초 단위 표시 시간
				/* Color */   FColor::Green,
				/* Message */ Message
			);
		}

		// ② 기존 로그도 남기고 싶으면
		UE_LOG(LogLogic, Log, TEXT("%s"), *Message);

		/////////////////////////////////////////////////
		// 인벤토리에 Data넘기기
		Cast<AQPlayer>(GetPawn())->AddEvidence(Data);

		// !! 오브젝트 삭제하기
		Cast<AQPlayer>(GetPawn())->ServerRPCPickUpEvidence(Evidence);

		return;
	}

	/*대화시작해도돼? */	
	//0. 상대방 NPC를 불러옴
	TObjectPtr<AQDynamicNPC> NPC = Cast<AQDynamicNPC>(_Player->GetClosestNPC());
	if (NPC != NULL) {
		//1. 물어봄
		Cast<AQPlayer>(GetPawn())->ServerRPCCanStartConversP2N(this, NPC);
		return;
	}
	
	
}

void AQPlayerController::InputTurnOnOffMap(const FInputActionValue& InputValue)
{
	 
	TObjectPtr<UQMapWidget> MapWidget = Cast<UQMapWidget>((VillageUIManager->GetActivedWidget(EVillageUIType::Map)));

	//보이는 상태가 아니면 켜기
	if (MapWidget->GetVisibility() != ESlateVisibility::Visible) {
		VillageUIManager->TurnOnUI(EVillageUIType::Map);
	}
	//보이면 끄기
	else {
		VillageUIManager->TurnOffUI(EVillageUIType::Map);
	}
	
}

void AQPlayerController::InputTurnOnOffInventory(const FInputActionValue& InputValue)
{

	
	TObjectPtr<UQInventoryWidget> InventoryWidget = Cast<UQInventoryWidget>((VillageUIManager->GetActivedWidget(EVillageUIType::Inventory)));

	//보이는 상태가 아니면 켜기
	if (InventoryWidget->GetVisibility() != ESlateVisibility::Visible) {
		VillageUIManager->TurnOnUI(EVillageUIType::Inventory);
	}
	//보이면 끄기
	else {
		VillageUIManager->TurnOffUI(EVillageUIType::Inventory);
	}
	
}

void AQPlayerController::InputTurn(const FInputActionValue& InputValue)
{
	//bEnableTurn이 true일때만 화면전환이 가능하다.
	if (bEnableTurn) {
		//내 고개 회전값을 control rotation에 전달. : 상하좌우 고갯짓
		float Yaw = InputValue.Get<FVector2D>().X;
		float Pitch = InputValue.Get<FVector2D>().Y;
		this->AddYawInput(Yaw);
		this->AddPitchInput(-Pitch);
	}
}



void AQPlayerController::InputMove(const FInputActionValue& InputValue)
{
	
	FVector2D MovementVector = InputValue.Get<FVector2D>();

	//앞 이동
	if (MovementVector.X != 0) {
		/*내가 바라보는 곳 기준의 앞 방향을 구해줌*/
		//마우스 방향 회전값 기준
		FRotator Rotator = this->GetControlRotation();
		//해당 회전값의 기준의 앞 방향을 구해준다.
		FVector Direction = UKismetMathLibrary::GetForwardVector(FRotator(0, Rotator.Yaw, 0));
		//방향과 크기를 전달함. (시간마다 거리를 더하는 것은 MovementComponent에서, 속도 세팅은 movementcomponent에서)
		this->GetPawn()->AddMovementInput(Direction, MovementVector.X);
	}
	//옆 이동
	if (MovementVector.Y != 0) {
		/*내가 바라보는 곳 기준의 옆 방향을 구해줌*/
		//마우스 방향 회전값 기준
		FRotator Rotator = this->GetControlRotation();
		//해당 회전값의 기준의 옆 방향을 구해준다.
		FVector Direction = UKismetMathLibrary::GetRightVector(FRotator(0, Rotator.Yaw, 0));
		//방향과 크기를 전달함. (시간마다 거리를 더하는 것은 MovementComponent에서, 속도 세팅은 movementcomponent에서)
		this->GetPawn()->AddMovementInput(Direction, MovementVector.Y);
	}
}



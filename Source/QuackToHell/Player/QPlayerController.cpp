// Copyright_Team_AriAri


#include "Player/QPlayerController.h"
#include "QLogCategories.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "UI/QVillageUIManager.h"
#include "UI/QMapWidget.h"
#include "UI/QP2NWidget.h"
#include "EnhancedInputComponent.h"
#include "UI/QInventoryWidget.h"
#include "Character/QPlayer.h"
#include "Character/QNPC.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NPC/QNPCController.h"
#include "Player/QPlayerState.h"
#include "QGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"


void AQPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Player로부터 EnhancedInput subsystem을 가져온다. 
	auto* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (SubSystem) {
		//mapping context를 추가해주는데, priority는 0
		SubSystem->AddMappingContext(InputMappingContext, 0);
	}

	//villagemanager를 가져온다
	VillageUIManager = AQVillageUIManager::GetInstance(GetWorld());


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





void AQPlayerController::StartDialog()
{

	//0. 상대방 NPC를 불러옴
	TObjectPtr<AQPlayer> _Player = Cast<AQPlayer>(this->GetPawn());
	TObjectPtr<AQNPC> NPC = Cast<AQNPC>(_Player->GetClosestNPC());
	//1. 대화 가능한지 check (서버에게 질문)
	/** @todo 유진 - 이 부분을 Player.cpp에 표시해놓은 부분에서 호출하면 될듯. + nullcheck같은거 추가로 하면ㄷ 좋을 것 같아요*/
	/*
	if (Cast<AQPlayer>(this->GetPawn())->GetCanStartConversP2N(NPC))
	{
		//1. UI를 킨다.
		VillageUIManager->TurnOnUI(EVillageUIType::P2N);

		//2. 상대방 NPC의 컨트롤러를 불러옴
		TObjectPtr<AQNPCController> NPCController = Cast<AQNPCController>(NPC->GetController());

		//3. NPC에게 대화 시작하라고 명령한다.
		NPCController->StartDialog(GetPawn(), ENPCConversationType::P2N);

		//4. 플레이어를 대화처리한다. (클라이언트)
		this->ConverseProcess();
		//4.1 플레이어를 대화처리한다. (서버)
		Cast<AQPlayer>(this->GetPawn())->StartConversation(NPC);

		//5. P2N Widget에게 자신의 정보를 넘긴다.
		//내 정보 넘겨주기
		Cast<UQP2NWidget>((VillageUIManager->GetVillageWidgets())[EVillageUIType::P2N])->SetConversingPlayer(this);
	}
	**/
}

void AQPlayerController::EndDialog()
{
	//0. 대화중의 상태를 remove

	//1. 상대방 NPC를 불러옴
	TObjectPtr<AQPlayer> _Player = Cast<AQPlayer>(this->GetPawn());
	//2. 상대방 NPC의 컨트롤러를 불러옴
	TObjectPtr<AQNPC> NPC = Cast<AQNPC>(_Player->GetClosestNPC());
	TObjectPtr<AQNPCController> NPCController = Cast<AQNPCController>(NPC->GetController());

	//3. 대화 그만하라고 명령한다.
	NPCController->EndDialog();
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

void AQPlayerController::ConverseProcess()
{
	//1. 몸 멈추기
	FreezePawn();
}

void AQPlayerController::ConverseEndProcess(TObjectPtr<class AQNPC> NPC)
{
	//1. 얼음땡
	UnFreezePawn();
	//2. 상태전환
	/** @todo 유진 - 이 부분을 Player.cpp에 표시해놓은 부분에서 호출하면 될듯. + nullcheck같은거 추가로 하면ㄷ 좋을 것 같아요*/
	/*
	Cast<AQPlayer>(GetPawn())->FinishConversation(NPC);
	*/
}

void AQPlayerController::InputEnableTurn(const FInputActionValue& InputValue)
{
	bEnableTurn = InputValue.Get<bool>() ? true : false;
}

void AQPlayerController::InputInteraction(const FInputActionValue& InputValue)
{
	
	//대화시작해라
	StartDialog();
	UE_LOG(LogLogic, Log, TEXT("E버튼 누름!"));
	
}

void AQPlayerController::InputTurnOnOffMap(const FInputActionValue& InputValue)
{

	

	//지도가 켜져있는지, 꺼져있는지에 따라 켤지끌지가 결정됨.
	//지도가 있는지부터 확인
	if (VillageUIManager->GetVillageWidgets().Contains(EVillageUIType::Map)) {
		TObjectPtr<UQMapWidget> MapWidget = Cast<UQMapWidget>((VillageUIManager->GetVillageWidgets())[EVillageUIType::Map]);

		//보이는 상태가 아니면 켜기
		if (MapWidget->GetVisibility() != ESlateVisibility::Visible) {
			VillageUIManager->TurnOnUI(EVillageUIType::Map);
		}
		//보이면 끄기
		else {
			VillageUIManager->TurnOffUI(EVillageUIType::Map);
		}
	}
	else {
		//지도없으면 UI켜기(생성)
		VillageUIManager->TurnOnUI(EVillageUIType::Map);
	}
	
}

void AQPlayerController::InputTurnOnOffInventory(const FInputActionValue& InputValue)
{
	//인벤토리가 켜져있는지, 꺼져있는지에 따라 켤지끌지가 결정됨.
	//인벤토리가 있는지부터 확인
	if (VillageUIManager->GetVillageWidgets().Contains(EVillageUIType::Inventory)) {
		TObjectPtr<UQInventoryWidget> InventoryWidget = Cast<UQInventoryWidget>((VillageUIManager->GetVillageWidgets())[EVillageUIType::Inventory]);

		//보이는 상태가 아니면 켜기
		if (InventoryWidget->GetVisibility() != ESlateVisibility::Visible) {
			VillageUIManager->TurnOnUI(EVillageUIType::Inventory);
		}
		//보이면 끄기
		else {
			VillageUIManager->TurnOffUI(EVillageUIType::Inventory);
		}
	}
	else {
		//지도없으면 UI켜기(생성)
		VillageUIManager->TurnOnUI(EVillageUIType::Inventory);
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



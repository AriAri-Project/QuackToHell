// Copyright_Team_AriAri

/** @todo id말고 이름으로 띄우도록 */
#include "UI/QRecordWidget.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "QLogCategories.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "UI/QVillageUIManager.h"
#include "Algo/Sort.h"
#include "UI/QDefaultVillageWidget.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/QChatBoxWidget.h"
#include "Player/QPlayerState.h"
UQRecordWidget::UQRecordWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/*ChatBox 위젯 블루프린트 클래스정보 가져오기*/
	static ConstructorHelpers::FClassFinder<UQChatBoxWidget> ChatBoxWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_ChatBox.WBP_ChatBox_C'"));
	if (ChatBoxWidgetAsset.Succeeded()) {
		ClassOfChatBox = ChatBoxWidgetAsset.Class;
	}
}

void UQRecordWidget::NativeConstruct()
{
	/*챗 히스토리 크기지정*/
	ChatHistories.SetNum(GhostMaxNum);

	/*델리게이트 바인딩*/
	TObjectPtr<UQDefaultVillageWidget> DefaultVillageWidet = Cast<UQDefaultVillageWidget>((AQVillageUIManager::GetInstance(GetWorld()))->GetActivedVillageWidgets()[EVillageUIType::DefaultVillageUI]);
	UE_LOG(LogLogic, Log, TEXT("RecordWidget - 디폴트위젯: %s"), *DefaultVillageWidet.GetName());
	DefaultVillageWidet->OnRecordButtonPressed.AddDynamic(this, &UQRecordWidget::UpdateRecordHistory);
	UE_LOG(LogLogic, Log, TEXT("RecordWidget - UpdateRecordHistory 바인딩 완료! 현재 바인딩 개수: %d"),
		DefaultVillageWidet->OnRecordButtonPressed.GetAllObjects().Num());
	

	/*버튼 정보 가져오기*/
	for (int8 i = 0; i < GhostMaxNum; i++) {
		/*FString포맷팅(출력아님)*/
		FString ButtonName = FString::Printf(TEXT("Button_%d"), i);
		FString OverlayName = FString::Printf(TEXT("ButtonOverlay_%d"), i);

		/*하위위젯가져오기 (GetWidgetFromName의 반환값은 UserWidget)*/
		//하위위젯컴포넌트들은 전부 UUserWidget을 상속받으므로 ㄱㅊ
		TObjectPtr<UButton> Button = Cast<UButton>(GetWidgetFromName(*ButtonName));
		TObjectPtr<UOverlay> Overlay = Cast<UOverlay>(GetWidgetFromName(*OverlayName));
		
		if (Button && Overlay) {
			FRecordButton  Temp;
			Temp.Button = Button;
			Temp.Overlay = Overlay;
			/*이름블록 가져오기*/
			TObjectPtr<UTextBlock> GhostName = nullptr;
			for (int8 ChildIndex = 0; ChildIndex < Overlay->GetChildrenCount(); ChildIndex++) {
				TObjectPtr<UWidget> ChildWidget = Overlay->GetChildAt(ChildIndex);
				//캐스팅성공시 가져오기
				GhostName = Cast<UTextBlock>(ChildWidget);
				if (GhostName) {
					break;
				}
			}
			//이름 아직도 없으면
			if (!GhostName) {
				UE_LOG(LogLogic, Error, TEXT("이름블록을 찾을 수 없습니다!"));
			}
			Temp.GhostName = GhostName;
			Buttons.Add(Temp);
		}
	}

	/* 녹음기 키기 전 대화할 경우 대비: 대화기록 첨부터 업데이트 */
	//녹음기 버튼에 바인딩했으므로, 처음 녹음기를 킬 경우 녹음기 인벤 위젯 생성 시점보다 UpdateRecordHistory가 앞서기 때문이다.
	UpdateRecordHistory();
	
}

void UQRecordWidget::UpdateRecordHistory()
{
	/*대화 레코드 정보가져오기*/
	APlayerController* PlayerController = GetOwningPlayer();
	AQPlayerState* PlayerState=nullptr;
	if (PlayerController) {
		PlayerState = PlayerController->GetPlayerState<AQPlayerState>();
		if (PlayerState) {
			ConversationRecord = PlayerState->GetRecordWithPlayerID();
		}
	}
	
	if (ConversationRecord.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("UQRecordWidget UpdateRecordHistory : ConversationRecord is empty"))
		return;
	}

	/*대화기록 정보 재구성: NPC별로, 시간순정렬*/
	//대화한 NPC목록들 저장
	//혹시 기존에 남아있는 게 있으면 초기화
	ConversedNPC.Empty();
	for (const auto & Value : ConversationRecord)
	{
		/*화자가 NPC인지 확인: 2000이상이면 NPC라고 가정*/
		if (Value.GetSpeakerID() >= 2000)
		{
			int32 SpeakerID = Value.GetSpeakerID();
			if (!ConversedNPC.Contains(SpeakerID)) {
				ConversedNPC.Add(SpeakerID);
			}
		}
		else if(Value.GetListenerID()>=2000)
		{
			int32 ListenerID = Value.GetListenerID();
			if (!ConversedNPC.Contains(ListenerID)) {
				ConversedNPC.Add(ListenerID);
			}
		}
	}

	/* 정렬된 대화기록을 저장할 맵: NPCID->정렬된 대화 배열: 초기화하여 다시 덮어쓰기 */
	SortedConversationRecord.Empty();

	for (int32 NPCID : ConversedNPC)
	{	
		TArray<FConversationRecord> NPCConversation;

		//해당 NPC와 관련된 대화 기록만 추출
		for (const FConversationRecord& Record : ConversationRecord) {
			bool bIsRelatedToNPC =
				(Record.GetSpeakerID() == NPCID || Record.GetListenerID() == NPCID);
		
			if (bIsRelatedToNPC) {
				NPCConversation.Add(Record);
			}
		}

		//시간순 정렬 (Sort함수 오버라이딩과 비슷)
		//A<B 면 A가 앞으로가고, 아니면 B가 앞으로간다
		//람다 : 익명함수
		//[]: 바깥변수 안 씀(캡쳐x)
		//두 인자를 바탕으로, {}안의 함수를 시행하겠다. 
		NPCConversation.Sort([](const FConversationRecord& A, const FConversationRecord& B) {
			return A.GetTimestamp() < B.GetTimestamp();
			});
		//맵에저장
		SortedConversationRecord.Add(NPCID, NPCConversation);
	}
	

	/*위젯업데이트: 버튼 클릭시 콜백*/
	/*전체 버튼 모양(8개 버튼들) 업데이트: 대화한 NPC 있을경우 유령모양 + NPC이름 뜨게*/
	UpdateButtonOverlay();
}

//들어올 때 버튼에 해당하는 NPC ID으로 들어옴. 그것으로부터 GetText해야함
void UQRecordWidget::TurnOnChatWidget(FText NPCID)
{
	/*이전 흔적 파괴: 채팅내용 새롭게 덮어쓰기*/
	//vertical box의 자식을 삭제한다. 
	VerticalBox->ClearChildren();

	/*버튼에 해당하는 NPC ID 추출 */
	int32 TargetNPCID = FCString::Atoi(*NPCID.ToString());

	/*SortedConversationRecord에 해당 NPC가 있는지 확인*/
	if (!SortedConversationRecord.Contains(TargetNPCID))
	{
		UE_LOG(LogLogic, Error, TEXT("UQRecordWidget TurnOnChatWidget: 해당 NPCID가 없습니다!"));
		return;
	}

	//NPCID키로 매핑하여 대화기록 가져옴
	const TArray<FConversationRecord>& ChatRecords = SortedConversationRecord[TargetNPCID];
	TObjectPtr<UWorld> World = GetWorld();
	if (!World) {
		return;
	}

	/*대화 기록 순서대로 위젯 생성*/
	for (const FConversationRecord& Record : ChatRecords) {
		//1. 채팅 위젯 생성
		TObjectPtr<UQChatBoxWidget> NewChatBox = CreateWidget<UQChatBoxWidget>(World, ClassOfChatBox);
		if (!NewChatBox) {
			continue;
		}

		NewChatBox->AddToViewport();
		NewChatBox->SetVisibility(ESlateVisibility::Visible);

		//2. 색상설정 (NPC: 연두색, Player: 흰색)
		bool bIsNPCSpeaking = (Record.GetSpeakerID() == TargetNPCID);
		FLinearColor TextColor = bIsNPCSpeaking
			? FLinearColor(0.886f, 1.0f, 0.890f, 1.0f)  // 연두색
			: FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);     // 흰색

		NewChatBox->SetColorAndOpacity(TextColor);

		//3. 메세지 설정
		TObjectPtr<UTextBlock> TextBox = Cast<UTextBlock>(NewChatBox->GetWidgetFromName(TEXT("Text")));
		if (TextBox) {
			TextBox->SetText(FText::FromString(Record.GetMessage()));
		}

		//4. 정렬 방향 설정
		TObjectPtr<UVerticalBoxSlot> NewSlot = VerticalBox->AddChildToVerticalBox(NewChatBox);
		if (NewSlot) {
			EHorizontalAlignment Alignment = bIsNPCSpeaking ? HAlign_Left : HAlign_Right;
			NewSlot->SetHorizontalAlignment(Alignment);

			NewSlot->SetPadding(FMargin(20.0f, 20.0f, 20.0f, 20.0f));
		}
	}
}

void UQRecordWidget::UpdateButtonOverlay()
{
	//대화한 NPC개수만큼 업데이트
	for (int8 i = 0; i < ConversedNPC.Num(); i++) {
		//오버레이켜주고
		Buttons[i].Overlay->SetVisibility(ESlateVisibility::Visible);
		//텍스트도업데이트
		int32 GhostID = ConversedNPC[i];
		FText GhostIDToText = FText::FromString(FString::FromInt(GhostID));
		Buttons[i].GhostName->SetText(GhostIDToText);
	}
	//나머지는 overlay끄기
	for (int8 i = ConversedNPC.Num(); i < GhostMaxNum; i++) {
		Buttons[i].Overlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

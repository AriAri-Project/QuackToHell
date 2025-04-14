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
	/*1. PlayerState에서 새로 들어온 대화 레코드 정보가져오기*/
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
	//2. 기존에 있는 SortedConversationRecord을 덮어쓰기 대신 누적 방식으로 유지
	for (const FConversationRecord& Record : ConversationRecord) {
		//누락방지: Speaker나 Listener 둘 중 하나가 NPC면 기록대상
		int32 PossibleNPCID = -1;

		if (Record.GetSpeakerID() >= 2000) {
			PossibleNPCID = Record.GetSpeakerID();
		}
		else if (Record.GetListenerID() >= 2000) {
			PossibleNPCID = Record.GetListenerID();
		}
		else {
			continue;//NPC가 안 껴있는대화는 생략
		}

		//NPC목록에 없으면 추가
		if (!ConversedNPC.Contains(PossibleNPCID))
		{
			ConversedNPC.Add(PossibleNPCID);
		}

		//NPC에 해당하는 배열이 없으면 새로 생성
		if (!SortedConversationRecord.Contains(PossibleNPCID))
		{
			{
				SortedConversationRecord.Add(PossibleNPCID, {});
			}
		}

		//중복 삽입 방지: 같은 ConversationID가 없을 때만 추가
		TArray<FConversationRecord>& RecordsForNPC = SortedConversationRecord[PossibleNPCID];
		bool bAlreadyExists = RecordsForNPC.ContainsByPredicate([&](const FConversationRecord& Existing) {
			return Existing.GetConversationID() == Record.GetConversationID();
		});
		if (!bAlreadyExists){
			RecordsForNPC.Add(Record);
		}
	}

	//3. 모든 NPC기록을 시간순정렬
	for (auto& Pair : SortedConversationRecord)
	{
		Pair.Value.Sort([](const FConversationRecord& A, const FConversationRecord& B) {
			return A.GetTimestamp() < B.GetTimestamp();
		});
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

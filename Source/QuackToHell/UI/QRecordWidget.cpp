// Copyright_Team_AriAri


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
	TObjectPtr<UQDefaultVillageWidget> DefaultVillageWidet = Cast<UQDefaultVillageWidget>((AQVillageUIManager::GetInstance(GetWorld()))->GetVillageWidgets()[EVillageUIType::DefaultVillageUI]);
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
			FMyButton  Temp;
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
	
}

void UQRecordWidget::UpdateRecordHistory()
{
	/*정보가져오기*/
	//월드로부터 가져오는 것이니 delete는 내가 신경쓰지x (세상에 하나만 존재)
	AQVillageGameState* const MyGameState = GetWorld() != NULL ? GetWorld()->GetGameState<AQVillageGameState>() : NULL;
	//값이므로 파괴주기 신경x (스택에쌓임)
	ConversationRecord = MyGameState->GetRecordWithPlayerID();

	/*대화기록 정보 업데이트*/
	{
		/*대화한 NPC기록 비우기: 중복 업데이트 방지*/
		ConversedNPC.Empty();
		/*더미데이터를 대화기록에 옮겨야됨.*/
		//정렬기준 1. NPCID, 정렬기준2: 시간순
		//람다를 사용해 정렬기준 지정해주기
		Algo::Sort(ConversationRecord,
			[](const FConversationRecord& A, const FConversationRecord& B) {
				//SpeackerID기준으로 우선정렬, 같으면 TimeStamp기준
				if (A.GetSpeakerID() != B.GetSpeakerID()) {
					//오름차순 (작은값이 앞으로 정렬)
					return A.GetSpeakerID() < B.GetSpeakerID();
				}
				return A.GetTimestamp() < B.GetTimestamp();
			});
		//대화기록에 옮기기
		int32 CachedNPCID = ConversationRecord[0].GetSpeakerID();
		ConversedNPC.Add(ConversationRecord[0].GetSpeakerID());
		int8 NPCIndex = 0;
		int8 ChatIndex = 0;
		//기본적으로 NPC가 먼저 말 시작한다.
		int8 IsNPC = 1;
		//대화개수만큼 대입
		for (int j = 0; j < ConversationRecord.Num(); j++) {
			//NPC바뀌면 변수값초기화
			if (CachedNPCID != ConversationRecord[j].GetSpeakerID()) {
				CachedNPCID = ConversationRecord[j].GetSpeakerID();
				//NPC변경
				NPCIndex++;
				//ChatIndex다시시작
				ChatIndex = 0;
				//NPC부터 대화시작(default)
				IsNPC = 1;
				//대화한 NPC목록에 추가
				ConversedNPC.Add(ConversationRecord[j].GetSpeakerID());
			}

			//대입 전 공간 추가
			FChat Temp;
			ChatHistories[NPCIndex].Add(Temp);
			//대입
			if (IsNPC == 1) {
				ChatHistories[NPCIndex][ChatIndex].ID = ConversationRecord[j].GetSpeakerID();
			}
			else {
				ChatHistories[NPCIndex][ChatIndex].ID = ConversationRecord[j].GetListenerID();
			}
			ChatHistories[NPCIndex][ChatIndex].Message = ConversationRecord[j].GetMessage();
			//플레이어턴으로 전환
			IsNPC *= -1;
			//챗 인덱스 ++
			ChatIndex++;
		}
	}

	/*위젯업데이트*/
	/*버튼*/
	UpdateButtonOverlay();
}

//들어올 때 NPC_0으로 들어옴. 그것으로부터 GetText해야함
void UQRecordWidget::TurnOnChatWidget(FText NPCID)
{
	/*이전 흔적 파괴*/
	//vertical box의 자식을 삭제한다. 
	VerticalBox->ClearChildren();

	/*채팅*/
	int32 _NPCID = FCString::Atoi(*NPCID.ToString());
	int8 IsNPC = 1;
	for (auto& Value : ConversationRecord)
	{
		if (Value.GetSpeakerID() != _NPCID) {
			continue;
		}

		//아이디찾으면
		{
			//1. 위젯 띄우기 (첫대화는항상NPC. )
				//색깔:  IsNPC에 따라 색깔다르게 (1이면 연두, -1이면 흰색)
				//메세지:  순서에맞게 할당
				//Alignment: IsNPC에따라. NPC면 왼쪽정렬, 아니면 오른쪽정렬
			
			
			/**
			 * @todo WBP를 생성해야함.
			 *
			 */
			TObjectPtr<UWorld> World = GetWorld();
			if (!World) return;
			
			TObjectPtr<UUserWidget> NewChatBox= CreateWidget<UUserWidget>(World, ClassOfChatBox);
			if (NewChatBox) {
				NewChatBox->AddToViewport();
				NewChatBox->SetVisibility(ESlateVisibility::Visible);
			}
			
			
			if (NewChatBox) {
				//색깔세팅
				if (IsNPC == 1) {
					FLinearColor MyColor;
					MyColor.R = 1.0f;
					MyColor.G = 1.0f;
					MyColor.B = 1.0f;
					MyColor.A = 1.0f;
					NewChatBox->SetColorAndOpacity(MyColor);
				}
				else {
					FLinearColor MyColor;
					MyColor.R = 0.886f;
					MyColor.G = 1.0f;
					MyColor.B = 0.890f;
					MyColor.A = 1.0f;
					NewChatBox->SetColorAndOpacity(MyColor);
				}
				
				//메세지세팅
				FString TextBlockName = TEXT("Text");
				TObjectPtr<UTextBlock> TextBox =  Cast<UTextBlock>(NewChatBox->GetWidgetFromName(*TextBlockName));
				TextBox->SetText(FText::FromString(Value.GetMessage()));
				
				
				//vertical box에 하위위젯으로 추가
				TObjectPtr<UVerticalBoxSlot> NewSlot = VerticalBox->AddChildToVerticalBox(NewChatBox);
				
				//Alignment설정
				if (NewSlot) {
					if (IsNPC==1) {
						NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
					}
					else {
						NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
					}
				}

				//Padding설정
				FMargin Margin;
				Margin.Left = 20;
				Margin.Right = 20;
				Margin.Top = 20;
				Margin.Bottom = 20;

				NewSlot->SetPadding(Margin);

				//IsNPC 반전처리
				IsNPC *= -1;
			}
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
		int32 SpeakerID = ConversedNPC[i];
		FText SpeackerIDToText = FText::FromString(FString::FromInt(SpeakerID));
		Buttons[i].GhostName->SetText(SpeackerIDToText);
	}
	//나머지는 overlay끄기
	for (int8 i = ConversedNPC.Num(); i < GhostMaxNum; i++) {
		Buttons[i].Overlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

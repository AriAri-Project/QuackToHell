// Copyright_Team_AriAri


#include "UI/QP2NWidget.h"
#include "Components/EditableTextBox.h"
#include "NPC/QNPCController.h"
#include "QLogCategories.h"

#include "Player/QPlayerController.h"
#include "Character/QPlayer.h"
#include "Components/TextBlock.h"
#include "Character/QDynamicNPC.h"
#include "NPC/QDynamicNPCController.h"
#include "NPCComponent.h"
#include "UI/QVillageUIManager.h"

void UQP2NWidget::UpdatePlayerText(const FString& Text)
{
    PlayerText->SetText(FText::FromString(Text));

}

void UQP2NWidget::UpdateNPCText(const FString& Text)
{
    NPCText->SetText(FText::FromString(Text));
}

void UQP2NWidget::SetConversingNPC(const TObjectPtr<class AQDynamicNPCController> NPC)
{
    ConversingNPC = NPC;

}

void UQP2NWidget::SetConversingPlayer(const TObjectPtr<class AQPlayerController> Player)
{
    ConversingPlayer = Player;
}

void UQP2NWidget::HandleEnterKeyPress()
{
    /*예외처리*/
    //공백문자열이면 무시한다. 
    if (inputBox->GetText().ToString() == TEXT("")) {
        UE_LOG(LogLogic, Log, TEXT("공백문자열을 input했습니다. 리턴합니다."));
        return;
    }
    ////////////////////////////////////////////////////
    //1 .플레이어 텍스트를 inputbox에 있던거로 업뎃한다.  
    UpdatePlayerText(inputBox->GetText().ToString());
    FString PlayerInput = inputBox->GetText().ToString();

    //2. inputbox는 공란으로 만든다.
    inputBox->SetText(FText::FromString(TEXT("")));
    //3. NPC Text는 음.. 으로 바꾼다.
    UpdateNPCText(WhenGenerateResponseText);
    //4. NPC에게 응답을 요청한다.
    ConversingNPC->Response(PlayerInput,EConversationType::P2N);
}


void UQP2NWidget::HandleEnterEndButton()
{
    //대화마치기 가능한지 서버에게 물어보기
    TObjectPtr<AQPlayer> _Player = Cast<AQPlayer>(ConversingPlayer->GetPawn());
    TObjectPtr<AQNPC> _NPC = Cast<AQNPC>(ConversingNPC->GetPawn());

    AQPlayerController* LocalPlayerController = Cast<AQPlayerController>(_Player->GetController());
    _Player->ServerRPCCanFinishConversP2N_Implementation(LocalPlayerController, _NPC);    
}



void UQP2NWidget::ClientRPCGetNPCResponse_Implementation(FOpenAIResponse NPCStartResponse)
{
    /** @todo 유진 : 서버측에서 NPC응답 왔을 때 실행할 함수 여기서 호출*/
    FString Response = NPCStartResponse.ResponseText;
    UpdateNPCText(Response);
}



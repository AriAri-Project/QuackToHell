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

void UQP2NWidget::SetConversingNPC(const TObjectPtr<AQDynamicNPC> NPC)
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
    //1-2. 서버에 플레이어 대사 저장
    TObjectPtr<AQPlayerController> ClientPC = Cast<AQPlayerController>(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
    AQPlayerState* PlayerState = ClientPC->GetPlayerState<AQPlayerState>();
    if (PlayerState == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UQP2NWidget::HandleEnterKeyPress - Can't find PlayerState."));
    }
    // 플레이어 응답 대화기록에 저장
    TObjectPtr<UNPCComponent> NPCComponent = ConversingNPC->FindComponentByClass<UNPCComponent>();
    int32 NPCID  = NPCComponent->GetNPCID();
    PlayerState->ServerRPCAddP2NPlayerStatement(EConversationType::P2N, NPCID, PlayerState->GetPlayerId(), PlayerInput);

    //2. inputbox는 공란으로 만든다.
    inputBox->SetText(FText::FromString(TEXT("")));
    //3. NPC Text는 음.. 으로 바꾼다.
    UpdateNPCText(WhenGenerateResponseText);
    //4. NPC에게 응답을 요청한다.
    /** @todo 이부분 AIController 사용하지 않도록 고치기 */
    ConversingNPC->GetResponse(ClientPC, PlayerInput, EConversationType::P2N);
}


void UQP2NWidget::HandleEnterEndButton()
{
    //대화마치기 가능한지 서버에게 물어보기
    TObjectPtr<AQPlayer> _Player = Cast<AQPlayer>(ConversingPlayer->GetPawn());
    TObjectPtr<AQNPC> _NPC = Cast<AQNPC>(ConversingNPC);
    
    if (_Player == nullptr && _NPC == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UQP2NWidget::HandleEnterEndButton - Can't find _Player, _NPC."));
    }

    AQPlayerController* LocalPlayerController = Cast<AQPlayerController>(_Player->GetController());
    _Player->ServerRPCCanFinishConversP2N_Implementation(LocalPlayerController, _NPC);    
}



void UQP2NWidget::DisplayNPCResponse(FOpenAIResponse NPCStartResponse)
{
    /** @todo 유진 : 서버측에서 NPC응답 왔을 때 실행할 함수 여기서 호출*/
    UE_LOG(LogTemp, Warning, TEXT("✅ DisplayNPCResponse : %s"), *NPCStartResponse.ResponseText);
    FString Response = NPCStartResponse.ResponseText;
    UpdateNPCText(Response);
}



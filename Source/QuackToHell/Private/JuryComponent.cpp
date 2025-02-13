// Copyright_Team_AriAri

#include "JuryComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "JsonObjectConverter.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

void UJuryComponent::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("UJuryComponent::BeginPlay() 실행됨 - NPC %s"), *NPCID);

    if (NPCID.IsEmpty())
        return;

    // 배심원의 JSON 파일 경로 설정
    int32 JuryIndex = FCString::Atoi(*NPCID) - 2000; // NPCID 2001 -> JuryIndex 1

    if (JuryIndex < 1 || JuryIndex > 3)
        return;

    FString PromptFileName = FString::Printf(TEXT("PromptToJury%d.json"), JuryIndex);
    PromptFilePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Prompt"), PromptFileName));

    UE_LOG(LogTemp, Log, TEXT("JuryComponent - NPC %s는 %s를 사용합니다."), *NPCID, *PromptFilePath);

    // 배심원 데이터 로드
    if (!NPCID.IsEmpty())
        LoadPrompt();

}

void UJuryComponent::AskJuryQuestion(const FString& PlayerInput)
{
    UE_LOG(LogTemp, Log, TEXT("Player asked the Jury: %s"), *PlayerInput);

    // FOpenAIRequest로 변환하여 StartConversation 호출
    FOpenAIRequest Request;
    Request.SpeakerID = FCString::Atoi(*GetPlayerIDAsString());
    Request.ListenerID = GetNPCID();  // NPC ID 가져오기
    Request.Prompt = PlayerInput;
    Request.MaxTokens = 150;
    Request.ConversationType = EConversationType::P2N;

    StartConversation(Request);
}

void UJuryComponent::StartConversation(const FOpenAIRequest& Request)
{
    if (PromptContent.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Prompt file is empty or failed to load for Jury NPC ID: %d"), Request.ListenerID);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Player started conversation with NPC %d: %s"), Request.ListenerID, *Request.Prompt);

    // 첫 대화인지 확인 (P2N 대화 기록이 없는 경우)
    bool bIsFirstGreeting = !P2NDialogueHistory.Contains(FString::Printf(TEXT("%d"), Request.ListenerID)) ||
        P2NDialogueHistory[FString::Printf(TEXT("%d"), Request.ListenerID)].DialogueLines.Num() == 0;

    FOpenAIRequest AIRequest;
    AIRequest.SpeakerID = Request.SpeakerID;
    AIRequest.ListenerID = Request.ListenerID;
    AIRequest.MaxTokens = 150;
    AIRequest.ConversationType = EConversationType::P2N;

    if (bIsFirstGreeting && Request.Prompt.IsEmpty())
    {
        // 첫 인사 생성 (배심원 설정 기반)
        AIRequest.Prompt = FString::Printf(TEXT(
            "아래 설정을 가진 배심원이 플레이어를 처음 만났을 때 하는 첫 인사를 생성하세요.\n"
            "==== 배심원 설정 ====\n%s\n"
            "첫 인사는 NPC의 성격과 설정을 반영하여 자연스럽게 작성해야 합니다."),
            *PromptContent);
    }
    else
    {
        // 일반적인 P2N 대화 처리
        AIRequest.Prompt = FString::Printf(TEXT(
            "아래 설정을 가진 배심원이 플레이어 '%d'의 질문에 답변합니다.\n"
            "==== 배심원 설정 ====\n%s\n"
            "==== 플레이어의 질문 ====\n"
            "플레이어: \"%s\"\n"
            "배심원:"),
            Request.SpeakerID, *PromptContent, *Request.Prompt);
    }

    RequestOpenAIResponse(AIRequest, [this, Request](FOpenAIResponse AIResponse)
        {
            ResponseCache.Add(Request.Prompt, AIResponse.ResponseText);
            UE_LOG(LogTemp, Log, TEXT("OpenAI Response for NPC %d: %s"), Request.ListenerID, *AIResponse.ResponseText);

            // 서버에 응답 전송
            SendNPCResponseToServer(AIResponse);

            // 대화 기록 저장
            SaveP2NDialogue(Request, AIResponse);
        });
}

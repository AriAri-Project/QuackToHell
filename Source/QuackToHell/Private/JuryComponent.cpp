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
    {
        // 현재 생성된 Resident 개수를 확인
        static int32 JuryCount = 0;

        NPCID = FString::FromInt(2001 + JuryCount);
        JuryCount++;

        UE_LOG(LogTemp, Log, TEXT("NPCID 자동 할당됨: %s"), *NPCID);
    }

    // 배심원의 JSON 파일 경로 설정
    int32 JuryIndex = FCString::Atoi(*NPCID) - 2000; // NPCID 2001 -> JuryIndex 1

    if (JuryIndex < 1 || JuryIndex > 3)
        return;

    FString PromptFileName = FString::Printf(TEXT("PromptToJury%d.json"), JuryIndex);
    PromptFilePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Prompt"), PromptFileName));

    UE_LOG(LogTemp, Log, TEXT("JuryComponent - NPC %s는 %s를 사용합니다."), *NPCID, *PromptFilePath);

    // 배심원 데이터 로드
    bool bLoaded = LoadPrompt();
    if (!bLoaded)
    {
        UE_LOG(LogTemp, Error, TEXT("프롬프트 로드 실패 - NPCID: %s, 파일: %s"), *NPCID, *PromptFilePath);
    }

    UE_LOG(LogTemp, Log, TEXT("🧐 BeginPlay 끝 - NPCID: %s, PromptContent 길이: %d"), *NPCID, PromptContent.Len());

}

void UJuryComponent::StartConversation(FOpenAIRequest Request)
{
    UE_LOG(LogTemp, Log, TEXT("🔵 JuryComponent::StartConversation 실행 - NPCID: %s"), *NPCID);
    UE_LOG(LogTemp, Log, TEXT("StartConversation 실행됨 - 현재 PromptContent 길이: %d"), PromptContent.Len());

    Request.SpeakerID = FCString::Atoi(*GetPlayerIDAsString());
    Request.ListenerID = GetNPCID();

    if (PromptContent.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("🚨 PromptContent가 비어 있음! NPCID: %s"), *NPCID);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Player started conversation with NPC %d: %s"), Request.ListenerID, *Request.Prompt);

    FString ListenerNPCID = FString::FromInt(Request.ListenerID);
    bool bIsFirstGreeting = !P2NDialogueHistory.Contains(ListenerNPCID) ||
        P2NDialogueHistory[ListenerNPCID].DialogueLines.Num() == 0;

    FOpenAIRequest AIRequest;
    AIRequest.SpeakerID = Request.SpeakerID;
    AIRequest.ListenerID = Request.ListenerID;
    AIRequest.MaxTokens = 150;

    // OpenAI가 JSON을 올바르게 해석할 수 있도록 변환
    FString ReadablePromptContent = ConvertJsonToReadableText(PromptContent);

    if (bIsFirstGreeting && Request.Prompt.IsEmpty())
    {
        AIRequest.ConversationType = EConversationType::PStart;
        AIRequest.Prompt = FString::Printf(TEXT(
            "{ \"model\": \"gpt-4o\", \"messages\": ["
            "{ \"role\": \"system\", \"content\": \"당신은 마을 NPC입니다. 다음은 NPC의 설정입니다: %s\" },"
            "{ \"role\": \"user\", \"content\": \"플레이어가 NPC를 처음 만났을 때 당신이 할 인사는?\" }],"
            "\"max_tokens\": 150 }"
        ), *ReadablePromptContent);
    }
    else
    {
        AIRequest.ConversationType = EConversationType::P2N;
        FString EscapedPlayerInput = Request.Prompt.Replace(TEXT("\n"), TEXT(" ")).Replace(TEXT("\""), TEXT("'"));
        AIRequest.Prompt = FString::Printf(TEXT(
            "{ \"model\": \"gpt-4o\", \"messages\": ["
            "{ \"role\": \"system\", \"content\": \"당신은 마을 NPC입니다. 다음은 NPC의 설정입니다: %s\" },"
            "{ \"role\": \"user\", \"content\": \"플레이어의 질문: '%s'\" }],"
            "\"max_tokens\": 150 }"
        ), *ReadablePromptContent, *EscapedPlayerInput);
    }

    UE_LOG(LogTemp, Log, TEXT("OpenAI 최종 요청 데이터(JSON): %s"), *AIRequest.Prompt);

    RequestOpenAIResponse(AIRequest, [this, Request](FOpenAIResponse AIResponse)
        {
            if (AIResponse.ResponseText.IsEmpty())
            {
                AIResponse.ResponseText = TEXT("죄송합니다, 질문에 답할 수 없습니다.");
            }

            ResponseCache.Add(Request.Prompt, AIResponse.ResponseText);
            SendNPCResponseToServer(AIResponse);
            SaveP2NDialogue(Request, AIResponse);
        });
}

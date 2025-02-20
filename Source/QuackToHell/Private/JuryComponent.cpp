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

        NPCID = FString::FromInt(2004 + JuryCount);
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
        FString EscapedPromptContent = PromptContent.Replace(TEXT("\n"), TEXT("\\n")).Replace(TEXT("\""), TEXT("\\\""));

        UE_LOG(LogTemp, Log, TEXT("StartConversation - 대화 유형: PStart"));
        AIRequest.ConversationType = EConversationType::PStart;

        // 첫 대사 생성 (NPC 설정을 기반으로 인사)
        AIRequest.Prompt = FString::Printf(TEXT(
            "{ \"model\": \"gpt-4o\", \"messages\": ["
            "{ \"role\": \"system\", \"content\": \"당신은 마을 NPC입니다. 플레이어를 처음 만났을 때의 첫 인사를 출력하세요. NPC의 설정을 반영하여 자연스럽게 작성해야 합니다.\" },"
            "{ \"role\": \"system\", \"content\": \"==== NPC 설정 ====\n%s\" },"
            "{ \"role\": \"user\", \"content\": \"플레이어가 NPC를 처음 만났을 때 당신이 할 인사는?\" }],"
            "\"max_tokens\": 150 }"
        ), *EscapedPromptContent);
    }
    else
    {
        FString EscapedPromptContent = PromptContent.Replace(TEXT("\n"), TEXT("\\n")).Replace(TEXT("\""), TEXT("\\\""));
        FString EscapedPlayerInput = Request.Prompt.Replace(TEXT("\n"), TEXT("\\n")).Replace(TEXT("\""), TEXT("\\\""));

        UE_LOG(LogTemp, Log, TEXT("StartConversation - 대화 유형: P2N, 플레이어 입력: %s"), *Request.Prompt);
        AIRequest.ConversationType = EConversationType::P2N;

        // 일반적인 P2N 대화 처리
        AIRequest.Prompt = FString::Printf(TEXT(
            "{ \"model\": \"gpt-4o\", \"messages\": ["
            "{ \"role\": \"system\", \"content\": \"당신은 마을 NPC입니다. 플레이어의 질문에 답변해야 하며, 다음 설정을 가지고 있습니다.\\n==== NPC 설정 ====\n%s\" },"
            "{ \"role\": \"system\", \"content\": \"플레이어 ID(참고용): %d\" },"
            "{ \"role\": \"user\", \"content\": \"플레이어의 질문: '%s'\" }],"
            "\"max_tokens\": 150 }"
        ), *EscapedPromptContent, Request.SpeakerID, *EscapedPlayerInput);
    }
    UE_LOG(LogTemp, Log, TEXT("📤 OpenAI 최종 요청 데이터(JSON): %s"), *AIRequest.Prompt);

    RequestOpenAIResponse(AIRequest, [this, Request](FOpenAIResponse AIResponse)
        {
            ResponseCache.Add(Request.Prompt, AIResponse.ResponseText);
            UE_LOG(LogTemp, Log, TEXT("OpenAI Response: %s"), *AIResponse.ResponseText);

            // 응답 서버 전송 (전체 응답 전달)
            SendNPCResponseToServer(AIResponse);

            // 대화 기록 저장
            SaveP2NDialogue(Request, AIResponse);
        });
}

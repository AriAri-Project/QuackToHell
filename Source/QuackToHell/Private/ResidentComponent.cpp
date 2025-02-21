// Copyright_Team_AriAri

#include "ResidentComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "JsonObjectConverter.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

#include "TimerManager.h"

void UResidentComponent::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("UResidentComponent::BeginPlay() 실행됨 - NPC %s"), *NPCID);

    if (NPCID.IsEmpty())
    {
        // 현재 생성된 Resident 개수를 확인
        static int32 ResidentCount = 0;

        NPCID = FString::FromInt(2004 + ResidentCount);
        ResidentCount++;

        UE_LOG(LogTemp, Log, TEXT("NPCID 자동 할당됨: %s"), *NPCID);
    }

    // 마을 주민의 JSON 파일 경로 설정
    int32 ResidentIndex = FCString::Atoi(*NPCID) - 2003; // NPCID 2004 -> ResidentIndex 1

    if (ResidentIndex < 1 || ResidentIndex > 5)
        return;

    FString PromptFileName = FString::Printf(TEXT("PromptToResident%d.json"), ResidentIndex);
    PromptFilePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Prompt"), PromptFileName));

    UE_LOG(LogTemp, Log, TEXT("ResidentComponent - NPC %s는 %s를 사용합니다."), *NPCID, *PromptFilePath);

    // 마을 주민 데이터 로드
    bool bLoaded = LoadPrompt();
    if (!bLoaded)
    {
        UE_LOG(LogTemp, Error, TEXT("프롬프트 로드 실패 - NPCID: %s, 파일: %s"), *NPCID, *PromptFilePath);
    }
}


void UResidentComponent::StartConversation(FOpenAIRequest Request)
{
    UE_LOG(LogTemp, Log, TEXT("StartConversation 실행됨 - 현재 PromptContent 길이: %d"), PromptContent.Len());

    Request.SpeakerID = FCString::Atoi(*GetPlayerIDAsString());
    Request.ListenerID = GetNPCID();

    if (PromptContent.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("PromptContent가 비어 있음! NPCID: %s"), *NPCID);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Player started conversation with NPC %d: %s"), Request.ListenerID, *Request.Prompt);

    FString ReadablePromptContent = UNPCComponent::ConvertJsonToReadableText(PromptContent);
    UE_LOG(LogTemp, Log, TEXT("변환된 NPC 설정: %s"), *ReadablePromptContent);

    FOpenAIRequest AIRequest;
    AIRequest.SpeakerID = Request.SpeakerID;
    AIRequest.ListenerID = Request.ListenerID;
    AIRequest.MaxTokens = 150;

    FString ListenerNPCID = FString::FromInt(Request.ListenerID);
    // 첫 대화인지 확인 (플레이어와의 P2N 대화 기록이 없는 경우)
    bool bIsFirstGreeting = !P2NDialogueHistory.Contains(ListenerNPCID) ||
        P2NDialogueHistory[ListenerNPCID].DialogueLines.Num() == 0;

    // JSON 객체 생성
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
    RootObject->SetStringField("model", "gpt-4o");

    TArray<TSharedPtr<FJsonValue>> Messages;

    // system 메시지 추가
    TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject());
    SystemMessage->SetStringField("role", "system");

    // PStart (첫 대화)
    if (bIsFirstGreeting && Request.Prompt.IsEmpty())
    {
        AIRequest.ConversationType = EConversationType::PStart;

        SystemMessage->SetStringField("content",
            FString::Printf(TEXT("당신은 마을 NPC입니다. 플레이어가 처음 당신과 대화할 때, "
                "친절하고 자연스러운 첫 인사를 해야 합니다. 다음은 당신의 설정입니다.\n%s"),
                *ReadablePromptContent));
    }
    // P2N (일반 대화)
    else
    {
        AIRequest.ConversationType = EConversationType::P2N;

        SystemMessage->SetStringField("content",
            FString::Printf(TEXT("당신은 마을 NPC입니다. 다음은 당신의 설정입니다.\n%s"),
                *ReadablePromptContent));
    }

    Messages.Add(MakeShareable(new FJsonValueObject(SystemMessage)));

    // user 메시지 추가
    TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject());
    UserMessage->SetStringField("role", "user");

    if (bIsFirstGreeting && Request.Prompt.IsEmpty())
    {
        UserMessage->SetStringField("content", "안녕하세요! 당신은 누구인가요?");
    }
    else
    {
        UserMessage->SetStringField("content",
            FString::Printf(TEXT("플레이어의 질문: '%s'"), *Request.Prompt));
    }

    Messages.Add(MakeShareable(new FJsonValueObject(UserMessage)));
    RootObject->SetArrayField("messages", Messages);
    RootObject->SetNumberField("max_tokens", 150);

    // JSON을 문자열로 변환
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    AIRequest.Prompt = RequestBody;

    UE_LOG(LogTemp, Log, TEXT("OpenAI 최종 요청 데이터(JSON): %s"), *RequestBody);

    RequestOpenAIResponse(AIRequest, [this, Request](FOpenAIResponse AIResponse)
        {
            if (AIResponse.ResponseText.IsEmpty())
            {
                AIResponse.ResponseText = TEXT("죄송합니다, 질문에 답할 수 없습니다.");
            }

            AIResponse.ConversationType = Request.ConversationType;
            ResponseCache.Add(Request.Prompt, AIResponse.ResponseText);
            SendNPCResponseToServer(AIResponse);
            SaveP2NDialogue(Request, AIResponse);
        });
}
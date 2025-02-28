// Copyright_Team_AriAri

#include "DefendantComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "JsonObjectConverter.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Logging/LogMacros.h"

void UDefendantComponent::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("UDefendantComponent::BeginPlay() 실행됨 - NPC %s"), *NPCID);

    static int32 DefendantCounter = 2001;

    if (NPCID.IsEmpty())
    {
        NPCID = FString::FromInt(DefendantCounter);
        UE_LOG(LogTemp, Log, TEXT("NPCID 자동 할당됨: %s"), *NPCID);
    }

    // 피고인의 JSON 파일 경로 설정
    PromptFilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Prompt"), TEXT("PromptToDefendant.json"));

    UE_LOG(LogTemp, Log, TEXT("DefendantComponent - NPC %s는 %s를 사용합니다."), *NPCID, *PromptFilePath);

    // 피고인 데이터 로드
    if (!NPCID.IsEmpty())
        LoadPrompt();
}

void UDefendantComponent::PerformNPCLogic()
{
    UE_LOG(LogTemp, Log, TEXT("Defendant is preparing their case."));

    if (PromptContent.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Defendant's prompt file."));
        return;
    }

    // 피고인의 JSON 데이터를 로그로 출력 (디버깅용)
    UE_LOG(LogTemp, Log, TEXT("Defendant's prompt loaded: %s"), *PromptContent);
}

void UDefendantComponent::DefendantOpeningStatement(FOpenAIRequest Request)
{
    UE_LOG(LogTemp, Log, TEXT("피고인의 모두진술 시작"));

    Request.SpeakerID = GetNPCID(); // 피고인은 2000
	Request.ListenerID = FCString::Atoi(*GetPlayerIDAsString());

    if (PromptContent.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("PromptContent가 비어 있음! NPCID: %s"), *NPCID);
        return;
    }

    FString ReadablePromptContent = UNPCComponent::ConvertJsonToReadableText(PromptContent);
    FString EscapedPlayerInput = Request.Prompt;

    FOpenAIRequest AIRequest;
    AIRequest.SpeakerID = Request.SpeakerID;
    AIRequest.ListenerID = Request.ListenerID;
    AIRequest.MaxTokens = 250;
    AIRequest.ConversationType = EConversationType::OpeningStatement;

    // JSON 객체 생성
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
    RootObject->SetStringField("model", "gpt-4o");

    TArray<TSharedPtr<FJsonValue>> Messages;

    // system 메시지 추가
    TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject());
    SystemMessage->SetStringField("role", "system");
    SystemMessage->SetStringField("content",
        FString::Printf(TEXT("당신은 천국행vs지옥행 재판을 받는 피고인입니다. 당신이 기억하는 당신의 전생이 어땠는지, 당신에게 재판이 유리해지도록 진술하세요: %s"),
            *ReadablePromptContent));
    Messages.Add(MakeShareable(new FJsonValueObject(SystemMessage)));

    RootObject->SetArrayField("messages", Messages);
    RootObject->SetNumberField("max_tokens", 250);

    // JSON을 문자열로 변환 후 AIRequest.Prompt에 저장
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    AIRequest.Prompt = RequestBody;

    UE_LOG(LogTemp, Log, TEXT("OpenAI 최종 요청 데이터(JSON): %s"), *RequestBody);

    RequestOpenAIResponse(AIRequest, [this](FOpenAIResponse AIResponse)
        {
            if (AIResponse.ResponseText.IsEmpty())
            {
                UE_LOG(LogTemp, Error, TEXT("AI 응답이 비어 있음!"));
                AIResponse.ResponseText = TEXT("응답이 비어 있습니다. DefendentComponent 확인 요망");
            }

            SendNPCResponseToServer(AIResponse);

        });
}


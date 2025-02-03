// Copyright_Team_AriAri


#include "NPCComponent.h"
#include "HttpModule.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Templates/Function.h"
#include "EngineUtils.h"  // 

// Sets default values for this component's properties
UNPCComponent::UNPCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UNPCComponent::BeginPlay()
{
	Super::BeginPlay();
	LoadPrompt();

}

// NPC의 프롬프트 파일 로드
void UNPCComponent::LoadPrompt()
{
	if (FFileHelper::LoadFileToString(PromptContent, *PromptFilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("Prompt loaded for NPC: %s"), *PromptContent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load prompt file: %s"), *PromptFilePath);
	}
}

/**
 * NPC 성격 Get/Set.
 */
FString UNPCComponent::GetNPCPersonality() const
{
	return NPCPersonality;
}

void UNPCComponent::SetNPCPersonality(const FString& NewPersonality)
{
	NPCPersonality = NewPersonality;
}

/**
 * NPC 대화 기록 Get/Set
 */
const TArray<FString>& UNPCComponent::GetNPCDialogueHistory() const
{
	return NPCDialogueHistory;
}

void UNPCComponent::AddToDialogueHistory(const FString& Dialogue)
{
	const int32 MaxHistorySize = 10;
	if (NPCDialogueHistory.Num() >= MaxHistorySize)
	{
		NPCDialogueHistory.RemoveAt(0); // 가장 오래된 대화 삭제
	}
	NPCDialogueHistory.Add(Dialogue);
}

/**
 * NPC 마지막 AI 응답 Get/Set
 */
 // NPC의 마지막 AI 응답 가져오기
FString UNPCComponent::GetLastAIResponse() const
{
	return LastAIResponse;
}

// NPC의 마지막 AI 응답 설정하기
void UNPCComponent::SetLastAIResponse(const FString& AIResponse)
{
	LastAIResponse = AIResponse;
}

// NPC 논리 실행 (자식 클래스에서 오버라이드 가능)
void UNPCComponent::PerformNPCLogic()
{
	// Base logic for NPCs (override this in child classes)
}

// OpenAI API 키 불러오기
FString UNPCComponent::GetAPIKey()
{
	FString ApiKey;
	GConfig->GetString(TEXT("OpenAI"), TEXT("ApiKey"), ApiKey, GGameIni);
	return ApiKey;
}

// OpenAI API 응답 JSON 파싱 함수
FOpenAIResponse UNPCComponent::ParseAIResponse(FString ResponseContent)
{
	return FOpenAIResponse::FromJson(ResponseContent);
}

// NPC 이름 가져오기
FString UNPCComponent::GetNPCName() const
{
	FString FilePath = PromptFilePath;
	FString FileContent;

	if (FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			return JsonObject->GetStringField("name");
		}
	}

	return TEXT("Unknown NPC");
}

// 플레이어가 NPC와 대화 시작
void UNPCComponent::StartConversation(const FString& PlayerInput)
{
	UE_LOG(LogTemp, Log, TEXT("Player started conversation: %s"), *PlayerInput);

	// OpenAI API에 요청 보내기
	if (bIsFirstConversation)
	{
		// 첫 대화일 경우, OpenAI에게 NPC의 첫 인사 요청
		RequestGreetingFromAI();
		bIsFirstConversation = false;
	}
	else
	{
		// 일반적인 대화 진행
		RequestAIResponse(PlayerInput);
	}
}

// OpenAI API 요청 처리
void UNPCComponent::RequestOpenAIResponse(const FOpenAIRequest& AIRequest, TFunction<void(FOpenAIResponse)> Callback)
{
	FString ApiKey = GetAPIKey();
	if (ApiKey.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("OpenAI API Key is missing!"));
		return;
	}

	TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL("https://api.openai.com/v1/completions");
	Request->SetVerb("POST");
	Request->SetHeader("Authorization", "Bearer " + ApiKey);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(AIRequest.ToJson());

	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			Callback(bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200
				? FOpenAIResponse::FromJson(Response->GetContentAsString())
				: FOpenAIResponse{ TEXT("죄송합니다, 답변할 수 없습니다.") });
		});

	Request->ProcessRequest();
}

// OpenAI API에 첫 인사 요청
void UNPCComponent::RequestGreetingFromAI()
{
	FOpenAIRequest AIRequest;
	AIRequest.Prompt = FString::Printf(TEXT("'%s' 성격을 가진 NPC가 첫 인사를 합니다. 첫 문장을 생성하세요."), *NPCPersonality);
	AIRequest.MaxTokens = 100;

	RequestOpenAIResponse(AIRequest, [this](FOpenAIResponse AIResponse)
		{
			UE_LOG(LogTemp, Log, TEXT("NPC Greeting Response: %s"), *AIResponse.ResponseText);

			// 서버 & 클라이언트 브로드캐스트
			SendNPCResponseToServer(AIResponse.ResponseText);
		});
}

TMap<FString, FString> ResponseCache;

// OpenAI API에 일반 대화 요청
void UNPCComponent::RequestAIResponse(const FString& PlayerInput)
{
	if (ResponseCache.Contains(PlayerInput))
	{
		FString CachedResponse = ResponseCache[PlayerInput];
		UE_LOG(LogTemp, Log, TEXT("Using cached AI response: %s"), *CachedResponse);
		SendNPCResponseToServer(CachedResponse);
		return;
	}

	FOpenAIRequest AIRequest;
	AIRequest.Prompt = FString::Printf(TEXT("'%s' 성격을 가진 NPC가 플레이어의 질문에 답변합니다.\n 플레이어: \"%s\"\n NPC:"),
		*NPCPersonality, *PlayerInput);
	AIRequest.MaxTokens = 150;

	RequestOpenAIResponse(AIRequest, [this, PlayerInput](FOpenAIResponse AIResponse)
		{
			ResponseCache.Add(PlayerInput, AIResponse.ResponseText);
			UE_LOG(LogTemp, Log, TEXT("OpenAI Response: %s"), *AIResponse.ResponseText);

			// 서버 & 클라이언트 브로드캐스트
			SendNPCResponseToServer(AIResponse.ResponseText);
		});
}

// NPC 간 대화 시작 (콜백 방식 적용)
void UNPCComponent::StartNPCToNPCDialog(const FString& FirstNPCID, const FString& SecondNPCID)
{
	UE_LOG(LogTemp, Log, TEXT("Starting dialog between %s and %s"), *FirstNPCID, *SecondNPCID);

	// 프롬프트 생성
	FString DialogPrompt = FString::Printf(
		TEXT("NPC '%s'와 NPC '%s'가 서로 대화를 나눕니다. 자연스럽고 연속적인 대화를 생성하세요."),
		*FirstNPCID, *SecondNPCID);

	// OpenAI API 호출
	FOpenAIRequest AIRequest;
	AIRequest.Prompt = DialogPrompt;
	AIRequest.MaxTokens = 150;  // 적절한 max_tokens을 설정할 것...!

	RequestOpenAIResponse(AIRequest, [this, FirstNPCID, SecondNPCID](FOpenAIResponse AIResponse)
		{
			UE_LOG(LogTemp, Log, TEXT("NPC Dialog Response: %s"), *AIResponse.ResponseText);
		});
}

// NPC 간 대화 응답 처리 (4턴)
void UNPCComponent::ContinueNPCToNPCDialog(
	const FString& SpeakerNPCID,
	const FString& ListenerNPCID,
	const FString& ReceivedMessage,  // 상대방 NPC가 방금 말한 내용
	int RemainingTurns)
{
	if (RemainingTurns <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("대화 종료!"));
		return;
	}

	// 최신 대화 히스토리를 구성
	FString DialogueHistory = "";
	for (const FString& History : NPCDialogueHistory)
	{
		DialogueHistory += "- " + History + TEXT("\n");
	}

	FString Prompt = FString::Printf(
		TEXT("NPC '%s'가 NPC '%s'의 말에 대답합니다.\n"
			"NPC '%s'가 방금 한 말: \"%s\"\n"
			"연속적인 대화를 자연스럽게 이어가세요.\n"
			"현재 대화 기록:\n%s\n"),
		*SpeakerNPCID, *ListenerNPCID, *SpeakerNPCID, *ReceivedMessage, *DialogueHistory);

	FOpenAIRequest AIRequest;
	AIRequest.Prompt = Prompt;
	AIRequest.MaxTokens = 150;  // 원하는 max_tokens 값 설정할 것!

	RequestOpenAIResponse(AIRequest, [this, SpeakerNPCID, ListenerNPCID, ReceivedMessage, RemainingTurns](FOpenAIResponse AIResponse)
		{
			FString NPCResponse = AIResponse.ResponseText;

			UE_LOG(LogTemp, Log, TEXT("%s: %s"), *SpeakerNPCID, *NPCResponse);

			// 대화 히스토리에 추가
			AddToDialogueHistory(FString::Printf(TEXT("%s: %s"), *SpeakerNPCID, *NPCResponse));

			// 다음 턴 진행 (Speaker와 Listener 역할 교체)
			ContinueNPCToNPCDialog(ListenerNPCID, SpeakerNPCID, NPCResponse, RemainingTurns - 1);
		});
}

// NPC가 플레이어와 대화한 내용을 저장
void UNPCComponent::SaveNPCDialogue(const FString& NPCResponse)
{
	// 최대 저장 개수 제한 (예: 최근 5개 대화만 저장)
	if (NPCDialogueHistory.Num() >= 5)
	{
		NPCDialogueHistory.RemoveAt(0); // 가장 오래된 대화 삭제
	}

	NPCDialogueHistory.Add(NPCResponse);
}

// NPC 혼잣말을 실행하는 함수
void UNPCComponent::PerformNPCMonologue()
{
	// 최근 대화 히스토리를 프롬프트에 추가
	FString MonologuePrompt = TEXT("NPC는 이전 대화를 요약하고, 이에 대한 혼잣말을 생성합니다. "
		"대화 기록:\n");

	for (const FString& Dialogue : NPCDialogueHistory)
	{
		MonologuePrompt += "- " + Dialogue + TEXT("\n");
	}

	MonologuePrompt += TEXT("\nNPC의 혼잣말: ");

	// OpenAI API 요청
	FOpenAIRequest AIRequest;
	AIRequest.Prompt = MonologuePrompt;
	AIRequest.MaxTokens = 100;

	RequestOpenAIResponse(AIRequest, [](FOpenAIResponse AIResponse)
		{
			UE_LOG(LogTemp, Log, TEXT("Generated Monologue: %s"), *AIResponse.ResponseText);
		});


}

FString UNPCComponent::GetLastDialogue() const
{
	if (LastDialogue.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No dialogue available in NPCComponent."));
		return TEXT("...");
	}

	return LastDialogue;
}

void UNPCComponent::ClientReceiveNPCResponse_Implementation(const FString& NPCResponse)
{
	UE_LOG(LogTemp, Log, TEXT("Client received NPC response: %s"), *NPCResponse);

	// UI에서 NPC 응답을 표시할 수 있도록 브로드캐스트
	OnNPCResponseReceived.Broadcast(NPCResponse);
}


// 서버로 NPC의 응답을 보내는 RPC 함수
void UNPCComponent::SendNPCResponseToServer_Implementation(const FString& NPCResponse)
{
	if (!NPCResponse.IsEmpty() && NPCResponse != TEXT("죄송합니다, 답변할 수 없습니다."))
	{

		FString PlayerName = TEXT("Unknown Player");  // 기본값 설정

		for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
		{
			if (PlayerController)
			{
				PlayerName = PlayerController->GetName();  // 루프 내에서 값 변경
				ClientRPCReceiveNPCResponse(PlayerName, NPCResponse);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("%s received NPC response: %s"), *PlayerName, *NPCResponse);

		UE_LOG(LogTemp, Log, TEXT("Sending NPC response to server: %s"), *NPCResponse);

		// 클라이언트에게도 응답을 전송
		ClientReceiveNPCResponse(NPCResponse);

		// 델리게이트를 활용하여 브로드캐스트 (서버 & 클라이언트에 전달)
		OnNPCResponseReceived.Broadcast(NPCResponse);

	}
}


bool UNPCComponent::SendNPCResponseToServer_Validate(const FString& NPCResponse)
{
	return true;
}


// Called every frame
void UNPCComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// -------------------------------------------------------------------------------------- //

void UNPCComponent::ClientRPCReceiveNPCResponse_Implementation(const FString& NPCResponse)
{
	UE_LOG(LogTemp, Log, TEXT("Client received NPC response: %s"), *NPCResponse);

	// 여기서 UI 업데이트하거나 NPC 대사를 표시하는 로직 추가 가능함.!
}

void UNPCComponent::ServerRPCGetGreeting_Implementation(const FString& NPCID)
{
}

void UNPCComponent::ServerRPCGetNPCResponseP2N_Implementation(const FString& NPCID, const FString& PlayerInput)
{
}

void UNPCComponent::ServerRPCGetGreetingN2N_Implementation(const FString& SpeakerNPCID, const FString& ListenerNPCID)
{
}

void UNPCComponent::ServerRPCGetNPCResponseN2N_Implementation(const FString& SpeakerNPCID, const FString& ListenerNPCID,
	const FString& NPCInput)
{
}

void UNPCComponent::ServerRPCGetNPCMonologue_Implementation(const FString& NPCID)
{
}

// ------------------------------------- //

FString UNPCComponent::GetNPCResponse(const FString& SpeakerNPCID, const FString& NPCInput,
	const FString& ListenerNPCID)
{
	FString NPCResponse = TEXT("temp response");
	return NPCResponse;
}
// Copyright_Team_AriAri


#include "NPCComponent.h"
#include "HttpModule.h"
#include "QLogCategories.h"
#include "Character/QPlayer.h"
#include "FramePro/FramePro.h"
#include "Game/QVillageGameState.h"
#include "Player/QPlayerController.h"
#include "Misc/FileHelper.h"
#include "UI/QVillageUIManager.h"
#include "UI/QP2NWidget.h"
#include "Misc/Paths.h"
#include "Templates/Function.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UNPCComponent::UNPCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	LastConversationTime = FDateTime::MinValue();
}

FString UNPCComponent::GetPlayerIDAsString() const
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPlayerIDAsString - PlayerController is nullptr!"));
		return TEXT("UnknownPlayer");
	}

	APlayerState* PS = PC->GetPlayerState<APlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPlayerIDAsString - PlayerState is nullptr!"));
		return TEXT("UnknownPlayer");
	}

	int32 PlayerID = PS->GetPlayerId();
	FString PlayerIDString = FString::FromInt(PlayerID);

	UE_LOG(LogTemp, Log, TEXT("GetPlayerIDAsString - PlayerID: %s"), *PlayerIDString);
	return PlayerIDString;
}

// Called when the game starts
void UNPCComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("NPCComponent - NPC %s BeginPlay 실행됨"), *GetOwner()->GetName());
}

void UNPCComponent::PerformNPCLogic()
{
	UE_LOG(LogTemp, Log, TEXT("Default NPC Logic"));
}

// NPC의 프롬프트 파일 로드
bool UNPCComponent::LoadPrompt()
{
	if (PromptFilePath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadPrompt 실행 전 PromptFilePath가 비어 있음! 기본 경로 설정 시도."));

		FString PromptFileName;

		// NPC 역할(role)에 따라 JSON 파일명 결정
		if (NPCRole == "resident")
		{
			PromptFileName = FString::Printf(TEXT("PromptToResident%d.json"), FCString::Atoi(*NPCID) - 2003);
		}
		else if (NPCRole == "jury")
		{
			PromptFileName = FString::Printf(TEXT("PromptToJury%d.json"), FCString::Atoi(*NPCID) - 2000);
		}
		else if (NPCRole == "defendant")
		{
			PromptFileName = TEXT("PromptToDefendant.json"); // Defendant는 단 하나만 존재
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LoadPrompt - 알 수 없는 NPC Role: %s"), *NPCRole);
			return false;
		}

		// 파일 경로 설정
		PromptFilePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Prompt"), PromptFileName));

		UE_LOG(LogTemp, Log, TEXT("LoadPrompt - NPCID: %s, Role: %s, 파일 경로: %s"), *NPCID, *NPCRole, *PromptFilePath);
	}

	FString FileContent;

	if (!FFileHelper::LoadFileToString(FileContent, *PromptFilePath) || FileContent.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("LoadPrompt 실패 (파일이 비어 있음) - %s"), *PromptFilePath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("JSON 파싱 실패 - %s"), *PromptFilePath);
		return false;
	}

	PromptContent = FileContent;

	// 필수 필드 확인
	if (JsonObject->HasField("npcid"))
	{
		NPCID = FString::FromInt(JsonObject->GetIntegerField("npcid"));
		UE_LOG(LogTemp, Log, TEXT("LoadPrompt - NPCID 할당 완료: %s"), *NPCID);
	}

	if (JsonObject->HasField("name"))
	{
		NPCName = JsonObject->GetStringField("name");
		UE_LOG(LogTemp, Log, TEXT("LoadPrompt - NPCName 할당 완료: %s"), *NPCName);
	}

	if (JsonObject->HasField("role"))
	{
		NPCRole = JsonObject->GetStringField("role");
		UE_LOG(LogTemp, Log, TEXT("LoadPrompt - NPCRole 할당 완료: %s"), *NPCRole);
	}

	UE_LOG(LogTemp, Log, TEXT("LoadPrompt 완료 - NPCID=%s, Name=%s, Role=%s"), *NPCID, *NPCName, *NPCRole);
	UE_LOG(LogTemp, Log, TEXT("PromptContent 저장 완료 - 내용: %s"), *PromptContent.Left(200));

	return true;
}

/**
 * NPC 성격 Get/Set.
 */
 //FString UNPCComponent::GetNPCPersonality() const
 //{
 //	return NPCPersonality;
 //}

 //void UNPCComponent::SetNPCPersonality(const FString& NewPersonality)
 //{
 //	NPCPersonality = NewPersonality;
 //}

// OpenAI API 키 불러오기
FString UNPCComponent::GetAPIKey()
{
	FString ConfigPath = FPaths::ProjectDir() + TEXT("OpenAIAuth.ini");
	FString FileContent;

	if (FFileHelper::LoadFileToString(FileContent, *ConfigPath))
	{
		UE_LOG(LogTemp, Log, TEXT("OpenAIAuth.ini 로드 성공"));
		FString ApiKey;
		if (FileContent.Split(TEXT("="), nullptr, &ApiKey))
		{
			return ApiKey.TrimStartAndEnd();
		}
	}

	UE_LOG(LogTemp, Error, TEXT("OpenAI API Key 로드 실패! OpenAIAuth.ini 확인 필요"));
	return TEXT("");
}

// OpenAI API 응답 JSON 파싱 함수
FOpenAIResponse UNPCComponent::ParseAIResponse(FString ResponseContent)
{
	return FOpenAIResponse::FromJson(ResponseContent);
}

// NPC 이름 가져오기
FString UNPCComponent::GetNPCName() const
{
	return NPCName;
}

int32 UNPCComponent::GetNPCID() const
{
	return FCString::Atoi(*NPCID);
}

bool UNPCComponent::GetIsFirstConversation() const
{
	return bIsFirstConversation;
}

bool UNPCComponent::GetIsRequestInProgress() const
{
	return bIsRequestInProgress;
}

FDateTime UNPCComponent::GetLastConversationTime() const
{
	return LastConversationTime;
}

void UNPCComponent::SetLastConversationTime(FDateTime NewLastConversationTime)
{
	LastConversationTime = NewLastConversationTime;
}

void UNPCComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNPCComponent, LastConversationTime);
}

// 현재 AI에게 요청을 보낼 수 있는지 없는지 판단
bool UNPCComponent::CanSendOpenAIRequest() const
{
	return !bIsRequestInProgress;
}

FString UNPCComponent::ConvertJsonToReadableText(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("🚨 JSON 파싱 실패! 원본 데이터 반환"));
		return JsonString;
	}

	FString ReadableText;

	// NPC 기본 정보
	if (JsonObject->HasField("name"))
	{
		ReadableText += FString::Printf(TEXT("이름: %s\n"), *JsonObject->GetStringField("name"));
	}

	if (JsonObject->HasField("npcid"))
	{
		ReadableText += FString::Printf(TEXT("NPC ID: %d\n"), JsonObject->GetIntegerField("npcid"));
	}

	if (JsonObject->HasField("personality"))
	{
		ReadableText += FString::Printf(TEXT("성격: %s\n"), *JsonObject->GetStringField("personality"));
	}

	if (JsonObject->HasField("speech_style"))
	{
		ReadableText += FString::Printf(TEXT("말투: %s\n"), *JsonObject->GetStringField("speech_style"));
	}

	if (JsonObject->HasField("situation_understanding"))
	{
		ReadableText += FString::Printf(TEXT("현재 상황 인식: %s\n"), *JsonObject->GetStringField("situation_understanding"));
	}

	if (JsonObject->HasField("past_life_relevance"))
	{
		ReadableText += FString::Printf(TEXT("과거 관련성: %s\n"), *JsonObject->GetStringField("past_life_relevance"));
	}

	if (JsonObject->HasField("past_life_story"))
	{
		ReadableText += FString::Printf(TEXT("과거 이야기: %s\n"), *JsonObject->GetStringField("past_life_story"));
	}

	if (JsonObject->HasField("knowledge_of_defendant's_past_life"))
	{
		ReadableText += FString::Printf(TEXT("피고인의 과거에 대한 지식: %s\n"), *JsonObject->GetStringField("knowledge_of_defendant's_past_life"));
	}

	return ReadableText;
}


// P2N 대화 시작
void UNPCComponent::StartConversation(FOpenAIRequest Request)
{
	UE_LOG(LogTemp, Log, TEXT("NPCComponent::StartConversation 실행 - NPCID: %s"), *NPCID);
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


// N2N 대화 시작
void UNPCComponent::StartNPCToNPCDialog(const FOpenAIRequest& Request)
{
	FString SpeakerNPCID = FString::FromInt(Request.SpeakerID);
	FString ListenerNPCID = FString::FromInt(Request.ListenerID);

	UE_LOG(LogTemp, Log, TEXT("Starting NPC-to-NPC dialog between %s and %s"), *SpeakerNPCID, *ListenerNPCID);

	FString PlayerDialogueSummary = TEXT("(플레이어와의 대화 기록 없음)");

	if (P2NDialogueHistory.Contains(SpeakerNPCID) && P2NDialogueHistory[SpeakerNPCID].DialogueLines.Num() > 0)
	{
		TArray<FString>& DialogueLines = P2NDialogueHistory[SpeakerNPCID].DialogueLines;
		int32 NumLines = DialogueLines.Num();

		FString RecentLines;
		int32 StartIndex = FMath::Max(0, NumLines - 3);
		for (int32 i = StartIndex; i < NumLines; i++)
		{
			RecentLines += DialogueLines[i] + TEXT(" ");
		}

		PlayerDialogueSummary = RecentLines.TrimEnd();
	}

	FOpenAIRequest AIRequest;
	AIRequest.Prompt = FString::Printf(
		TEXT("NPC '%s'가 NPC '%s'와의 대화를 시작합니다. "
			"최근 플레이어와 나눈 대화 기록: %s "
			"위 내용을 참고하여 상대 NPC와의 대화를 시작하기 위한 말을 꺼내세요."),
		*SpeakerNPCID, *ListenerNPCID, *PlayerDialogueSummary);

	AIRequest.MaxTokens = 150;
	AIRequest.SpeakerID = Request.SpeakerID;  // 그대로 유지
	AIRequest.ListenerID = Request.ListenerID;  // 그대로 유지
	AIRequest.ConversationType = EConversationType::N2NStart;

	RequestOpenAIResponse(AIRequest, [this, ListenerNPCID, SpeakerNPCID](FOpenAIResponse AIResponse)
		{
			SendNPCResponseToServer(AIResponse);
			ContinueNPCToNPCDialog(FOpenAIRequest(AIResponse.ListenerID, AIResponse.SpeakerID, EConversationType::N2N, AIResponse.ResponseText, 4));
		});

	UE_LOG(LogTemp, Log, TEXT("NPC-to-NPC 대화 요청 전송 완료"));
}


// N2N 대화 이어나감
void UNPCComponent::ContinueNPCToNPCDialog(const FOpenAIRequest& Request)
{
	if (Request.MaxTokens <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("N2N 대화가 종료되었습니다."));
		return;
	}

	FString SpeakerNPCID = FString::FromInt(Request.SpeakerID);
	FString ListenerNPCID = FString::FromInt(Request.ListenerID);

	// 최근 대화 내역을 반영하여 응답 생성
	FOpenAIRequest AIRequest;
	AIRequest.Prompt = FString::Printf(TEXT(
		"NPC '%s'가 NPC '%s'의 말에 대답합니다. 이때 자연스럽고 일관성 있는 대화를 이어가세요. NPC '%s'가 방금 한 말: \"%s\""),
		*SpeakerNPCID, *ListenerNPCID, *SpeakerNPCID, *Request.Prompt); // 이전 메시지를 Prompt로 전달

	AIRequest.MaxTokens = 150;
	AIRequest.SpeakerID = Request.SpeakerID;
	AIRequest.ListenerID = Request.ListenerID;
	AIRequest.ConversationType = EConversationType::N2N;

	// OpenAI API 호출 후, 대화 이어나가기 (남은 턴 수 감소)
	RequestOpenAIResponse(AIRequest, [this, Request](FOpenAIResponse AIResponse)
		{
			SendNPCResponseToServer(AIResponse);
			FOpenAIRequest NextRequest = Request;
			NextRequest.Prompt = AIResponse.ResponseText;  // AI 응답을 다음 메시지로 설정
			NextRequest.MaxTokens -= 1;  // 남은 턴 감소

			ContinueNPCToNPCDialog(NextRequest);
		});
}


// N혼잣말 생성
void UNPCComponent::PerformNPCMonologue(const FOpenAIRequest& Request)
{
	FString NPCIDString = FString(NPCID);
	FString Context;
	FString SpeakerNPCID = FString::FromInt(Request.SpeakerID);  // NPC ID 변환

	// P2N 대화 기록이 존재하는 경우에만!! 해당 NPC의 대화 기록을 기반으로 혼잣말 생성
	if (P2NDialogueHistory.Contains(SpeakerNPCID) && P2NDialogueHistory[SpeakerNPCID].DialogueLines.Num() > 0)
	{
		TArray<FString>& DialogueLines = P2NDialogueHistory[SpeakerNPCID].DialogueLines;
		int32 NumLines = DialogueLines.Num();

		FString RecentLines;
		int32 StartIndex = FMath::Max(0, NumLines - 3);
		for (int32 i = StartIndex; i < NumLines; i++)
		{
			RecentLines += DialogueLines[i] + TEXT(" ");
		}

		Context = RecentLines.TrimEnd();
	}
	else
	{
		Context = TEXT("플레이어와 대화를 나눈 적이 없는 NPC입니다.");
	}

	FOpenAIRequest AIRequest = Request; // 기존 요청 정보를 유지
	AIRequest.Prompt = FString::Printf(TEXT(
		"NPC '%s'가 혼잣말을 합니다. "
		"최근 대화 기록: %s "
		"이 대화를 바탕으로 자연스럽고 감정적인 독백을 생성하세요."),
		*SpeakerNPCID, *Context);

	AIRequest.MaxTokens = 100;
	AIRequest.ListenerID = 0;  // 혼잣말이므로 Listener 없음
	AIRequest.ConversationType = EConversationType::NMonologue;

	RequestOpenAIResponse(AIRequest, [this, SpeakerNPCID](FOpenAIResponse AIResponse)
		{
			SendNPCResponseToServer(AIResponse);
			UE_LOG(LogTemp, Log, TEXT("NPC %s가 혼잣말을 시작합니다.: %s"), *SpeakerNPCID, *AIResponse.ResponseText);
		});
}

// OpenAI API 요청 처리
void UNPCComponent::RequestOpenAIResponse(const FOpenAIRequest& AIRequest, TFunction<void(FOpenAIResponse)> Callback)
{
	if (!CanSendOpenAIRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenAI가 호출 중이므로 새로운 요청을 보낼 수 없습니다."));
		return;
	}

	bIsRequestInProgress = true;

	FString ApiKey = GetAPIKey();
	if (ApiKey.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("OpenAI API Key가 없습니다!"));
		bIsRequestInProgress = false;
		return;
	}

	FString RequestBody = AIRequest.Prompt;
	UE_LOG(LogTemp, Log, TEXT("OpenAI에 보낼 최종 JSON 데이터: %s"), *RequestBody);

	TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL("https://api.openai.com/v1/completions");
	Request->SetVerb("POST");
	Request->SetHeader("Authorization", "Bearer " + ApiKey);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(RequestBody);

	Request->OnProcessRequestComplete().BindLambda([this, Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			bIsRequestInProgress = false;

			if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
			{
				UE_LOG(LogTemp, Error, TEXT("OpenAI 응답 실패! 기본 응답 반환."));
				FOpenAIResponse FailedResponse;
				FailedResponse.ResponseText = TEXT("죄송합니다, 현재 답변할 수 없습니다.");
				Callback(FailedResponse);
				return;
			}

			FString ResponseContent = Response->GetContentAsString();
			UE_LOG(LogTemp, Log, TEXT("OpenAI 응답 수신: %s"), *ResponseContent);

			FOpenAIResponse AIResponse = FOpenAIResponse::FromJson(ResponseContent);
			Callback(AIResponse);
		});

	Request->ProcessRequest();
}

TMap<FString, FString> ResponseCache;

// OpenAI API에 일반 대화 요청
void UNPCComponent::RequestAIResponse(const FString& PlayerInput)
{

	// OpenAI API 요청
	FOpenAIRequest AIRequest;
	AIRequest.Prompt = FString::Printf(TEXT("'%s' 성격을 가진 NPC가 플레이어의 질문에 답변합니다.\n 플레이어: \"%s\"\n NPC:"),
		*NPCPersonality, *PlayerInput);
	AIRequest.MaxTokens = 150;

	RequestOpenAIResponse(AIRequest, [this, PlayerInput](FOpenAIResponse AIResponse)
		{
			ResponseCache.Add(PlayerInput, AIResponse.ResponseText);
			UE_LOG(LogTemp, Log, TEXT("OpenAI Response: %s"), *AIResponse.ResponseText);
			SendNPCResponseToServer(AIResponse);
			OnNPCResponseReceived.Broadcast(AIResponse.ResponseText);
		});
}

void UNPCComponent::SaveP2NDialogue(const FOpenAIRequest& Request, const FOpenAIResponse& Response)
{
	FString NPCIDString = FString::FromInt(Request.ListenerID);

	if (!P2NDialogueHistory.Contains(NPCIDString))
	{
		P2NDialogueHistory.Add(NPCIDString, FDialogueHistory());
	}

	// 플레이어 입력이 있을 경우에만 저장해야 함 (안 그러면 쓰레기 데이터가 생김)
	if (!Request.Prompt.IsEmpty())
	{
		P2NDialogueHistory[NPCIDString].DialogueLines.Add(FString::Printf(TEXT("Player: %s"), *Request.Prompt));
	}

	// NPC 응답은 바로 저장
	P2NDialogueHistory[NPCIDString].DialogueLines.Add(FString::Printf(TEXT("NPC(%d): %s"), Response.SpeakerID, *Response.ResponseText));

	UE_LOG(LogTemp, Log, TEXT("P2N 대화 기록 저장 완료 - NPC: %s"), *NPCIDString);
}



// GetP2NDialogueHistory("NPCID")를 호출하면 대화 기록을 가져올 수 있음
TArray<FString> UNPCComponent::GetP2NDialogueHistory() const
{
	if (P2NDialogueHistory.Contains(NPCID))
	{
		return P2NDialogueHistory[NPCID].DialogueLines;
	}

	UE_LOG(LogTemp, Warning, TEXT("GetP2NDialogueHistory: No conversation history found for NPC %s"), *NPCID);
	return TArray<FString>(); // 기록이 없으면 빈 배열 반환
}

// 서버로 NPC의 응답을 보내는 RPC 함수
// 서버로 NPC의 응답을 보내는 RPC 함수 (FOpenAIResponse 전체 전달)
void UNPCComponent::SendNPCResponseToServer_Implementation(const FOpenAIResponse& Response)
{
	UE_LOG(LogLogic, Log, TEXT("SendNPCResponseToServer_Implementation Started"));
	if (!Response.ResponseText.IsEmpty() && Response.ResponseText != TEXT("죄송합니다, 답변할 수 없습니다."))
	{
		UE_LOG(LogTemp, Log, TEXT("Sending NPC response to server: %s"), *Response.ResponseText);
	}

	switch (Response.ConversationType)
	{
	case EConversationType::PStart:
		UE_LOG(LogTemp, Log, TEXT("NPC의 첫 인사(PStart) 응답 처리"));
		break;
	case EConversationType::P2N:
		UE_LOG(LogTemp, Log, TEXT("P2N 대화 응답 처리"));
		break;
	case EConversationType::N2N:
		UE_LOG(LogTemp, Log, TEXT("N2N 대화 응답 처리"));
		break;
	case EConversationType::NMonologue:
		UE_LOG(LogTemp, Log, TEXT("NMonologue 대화 응답 처리"));
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("알 수 없는 ConversationType 처리 불가"));
		return;
	}

	if (Response.SpeakerID == 0 && Response.ListenerID == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UNPCComponent::OnSuccessGetNPCResponse -> Invalid Response"))
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("OnSuccessGetNPCResponse -> HasAuthority false."))
	}
	// 대화기록 저장
	TObjectPtr<AQVillageGameState> VillageGameState = Cast<AQVillageGameState>(GetWorld()->GetGameState());
	if (VillageGameState == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("OnSuccessStartConversation - VillageGameState is null."));
		return;
	}
	SetLastConversationTime(FDateTime::Now());
	int32 RecordID = VillageGameState->AddConversationRecord(Response.ConversationType, Response.ListenerID, Response.SpeakerID, LastConversationTime, Response.ResponseText);
	if (RecordID < 0)
	{
		return;
	}
	
	APlayerController* TargetPlayerController;
	TObjectPtr<UQP2NWidget> P2NWidget;
	switch (Response.ConversationType)
	{
	case EConversationType::PStart: // 임시
		TargetPlayerController = Cast<APlayerController>(GetOwner());
		if (TargetPlayerController)
		{
			Cast<AQPlayerController>(TargetPlayerController)->ClientRPCStartConversation(Response);
		}
		break;
	case EConversationType::P2N: // 임시
		P2NWidget = Cast<UQP2NWidget>(AQVillageUIManager::GetInstance(GetWorld())->GetActivedVillageWidgets()[EVillageUIType::P2N]);			
		if (P2NWidget)
		{
			P2NWidget->ClientRPCGetNPCResponse(Response);
		}
		break;
	case EConversationType::N2N:
		break;
	case EConversationType::N2NStart:
		break;
	case EConversationType::NMonologue:
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("GetNPCResponse -> Invaild ConversationType"));
		break;
	}
}

bool UNPCComponent::SendNPCResponseToServer_Validate(const FOpenAIResponse& AIResponse)
{
	return true; // 기본적으로 항상 유효한 메시지라고 가정
}

// Called every frame
void UNPCComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// -------------------------------------------------------------------------------------- //

void UNPCComponent::ServerRPCGetNPCResponse_Implementation(FOpenAIRequest Request)
{
	/*
	 *	PStart, P2N : StartConversation, SendNPCResponseToServer()
	 *	N2N 대화 시작 : StartNPCToNPCDialog, SendNPCResponseToServer()
	 *	N2N 대화 진행 : ContinueNPCToNPCDialog(), SendNPCResponseToServer()
	 *	NMonologue : PerformNPCMonologue(), SendNPCResponseToServer()
	 */
	
	UE_LOG(LogLogic, Log, TEXT("GetNPCResponse Started"));
	switch (Request.ConversationType)
	{
	case EConversationType::None:
		UE_LOG(LogTemp, Error, TEXT("GetNPCResponse -> Invaild ConversationType"));
		break;
	case EConversationType::N2N:
		ContinueNPCToNPCDialog(Request);
		break;
	case EConversationType::N2NStart:
		StartNPCToNPCDialog(Request);
		break;
	case EConversationType::NMonologue:
		PerformNPCMonologue(Request);
		break;
	default:	// type이 PStart나 P2N이라면 
		StartConversation(Request);
		break;
	}
}
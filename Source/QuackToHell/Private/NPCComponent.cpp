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

	// 필수 필드 확인
	if (!JsonObject->HasField("npcid") || !JsonObject->HasField("name"))
	{
		UE_LOG(LogTemp, Error, TEXT("필수 필드 누락 - %s"), *PromptFilePath);
		return false;
	}

	// NPCID를 정상적으로 읽어왔는지 확인
	int32 TempID = 0;
	if (JsonObject->TryGetNumberField("npcid", TempID))
	{
		NPCID = FString::FromInt(TempID);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NPCID 변환 실패!"));
		return false;
	}

	NPCName = JsonObject->GetStringField("name");

	UE_LOG(LogTemp, Log, TEXT("LoadPrompt 완료: NPCID=%s, Name=%s"), *NPCID, *NPCName);
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

// P2N 대화 시작
void UNPCComponent::StartConversation(const FOpenAIRequest& Request)
{
	if (PromptContent.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Prompt file is empty or failed to load for NPC: %d"), Request.ListenerID);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Player started conversation with NPC %d: %s"), Request.ListenerID, *Request.Prompt);

	FString ListenerNPCID = FString::FromInt(Request.ListenerID);
	// 첫 대화인지 확인 (플레이어와의 P2N 대화 기록이 없는 경우)
	bool bIsFirstGreeting = !P2NDialogueHistory.Contains(ListenerNPCID) ||
		P2NDialogueHistory[ListenerNPCID].DialogueLines.Num() == 0;


	// 기존 AIRequest 유지
	FOpenAIRequest AIRequest;
	AIRequest.SpeakerID = Request.SpeakerID;
	AIRequest.ListenerID = Request.ListenerID;
	AIRequest.MaxTokens = 150;

	if (bIsFirstGreeting && Request.Prompt.IsEmpty())
	{
		AIRequest.ConversationType = EConversationType::PStart;
		// 첫 대사 생성 (NPC 설정을 기반으로 인사)
		AIRequest.Prompt = FString::Printf(TEXT(
			"아래 설정을 가진 NPC가 플레이어를 처음 만났을 때 하는 첫 인사를 생성하세요.\n"
			"==== NPC 설정 ====\n%s\n"
			"첫 인사는 NPC의 성격과 설정을 반영하여 자연스럽게 작성해야 합니다."), *PromptContent);
	}
	else
	{
		AIRequest.ConversationType = EConversationType::P2N;
		// 일반적인 P2N 대화 처리
		AIRequest.Prompt = FString::Printf(TEXT(
			"아래 설정을 가진 NPC가 플레이어 '%d'의 질문에 답변합니다.\n"
			"==== NPC 설정 ====\n%s\n"
			"==== 플레이어의 질문 ====\n"
			"플레이어: \"%s\"\n"
			"NPC:"), Request.SpeakerID, *PromptContent, *Request.Prompt);
	}

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
		UE_LOG(LogTemp, Warning, TEXT("OpenAI가 호출 중이기 때문에 새로운 요청을 보낼 수 없습니다."));
		return;
	}

	// 새로운 요청 진행 가능이라면 true 상태로 변경
	bIsRequestInProgress = true;

	FString ApiKey = GetAPIKey();
	if (ApiKey.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("OpenAI API Key is missing!"));
		bIsRequestInProgress = false;
		return;
	}

	TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL("https://api.openai.com/v1/completions");
	Request->SetVerb("POST");
	Request->SetHeader("Authorization", "Bearer " + ApiKey);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(AIRequest.ToJson());

	Request->OnProcessRequestComplete().BindLambda([this, Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			// 요청 완료되면 진행 상태 해제하고 false로 변경
			bIsRequestInProgress = false;

			Callback(bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200
				? FOpenAIResponse::FromJson(Response->GetContentAsString())
				: FOpenAIResponse{ TEXT("콜백 함수 - 응답 불러오기에 실패했습니다.") });
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
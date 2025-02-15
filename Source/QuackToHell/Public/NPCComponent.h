// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GameData/QConversationData.h"
#include "Delegates/DelegateCombinations.h"
#include "NPCComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNPCResponseReceived, const FString&, ResponseText);

/**
 * @author 박시언
 * @brief OpenAI API 요청을 위한 구조체
 */
USTRUCT(BlueprintType)
struct FOpenAIRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	FString ResponseText;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	int32 SpeakerID;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	int32 ListenerID;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	EConversationType ConversationType;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	FString Prompt;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	int32 MaxTokens = 150;

	// 기본 생성자
	FOpenAIRequest(){};
	// 생성자
	FOpenAIRequest(int32 NewSpeakerID, int32 NewListenerID, EConversationType ConversationState, const FString& NewPrompt, int32 NewMaxTokens  = 150)
		: SpeakerID(NewSpeakerID), ListenerID(NewListenerID), ConversationType(ConversationState), Prompt(NewPrompt), MaxTokens(NewMaxTokens)
	{};
	
	// ToJson()은 public임
	FString ToJson() const
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		JsonObject->SetStringField("prompt", Prompt);
		JsonObject->SetNumberField("max_tokens", MaxTokens);
		JsonObject->SetNumberField("speaker_id", SpeakerID);
		JsonObject->SetNumberField("listener_id", ListenerID);
		JsonObject->SetStringField("conversation_type", ConversationTypeToString(ConversationType));

		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

		return OutputString;
	}

private:
	FString ConversationTypeToString(EConversationType Type) const
	{
		switch (Type)
		{
		case EConversationType::P2N: return TEXT("P2N");
		case EConversationType::N2N: return TEXT("N2N");
		case EConversationType::NMonologue: return TEXT("NMonologue");
		default: return TEXT("Unknown");
		}
	}
};

// ---------------------------------------------------------------------------------------------

/**
 * @author 박시언
 * @brief OpenAI API 응답을 위한 구조체
 */
USTRUCT(BlueprintType)
struct FOpenAIResponse
{
	GENERATED_BODY()

	FString ResponseText;
	int32 SpeakerID;
	int32 ListenerID;
	EConversationType ConversationType;

	static FOpenAIResponse FromJson(const FString& JsonContent)
	{
		FOpenAIResponse Response;
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
			if (JsonObject->TryGetArrayField("choices", ChoicesArray) && ChoicesArray->Num() > 0)
			{
				Response.ResponseText = (*ChoicesArray)[0]->AsString();
			}

			// SpeakerID, ListenerID, ConversationType 값
            Response.SpeakerID = JsonObject->GetIntegerField("speaker_id");
            Response.ListenerID = JsonObject->GetIntegerField("listener_id");

            FString ConversationTypeString = JsonObject->GetStringField("conversation_type");

            if (ConversationTypeString == "P2N") Response.ConversationType = EConversationType::P2N;
            else if (ConversationTypeString == "N2N") Response.ConversationType = EConversationType::N2N;
            else if (ConversationTypeString == "NMonologue") Response.ConversationType = EConversationType::NMonologue;
            else Response.ConversationType = EConversationType::P2N; 
		}

		if (Response.ResponseText.IsEmpty())
		{
			Response.ResponseText = TEXT("죄송합니다, 답변할 수 없습니다.");
		}

		return Response;
	}
};

// ---------------------------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FDialogueHistory
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
	TArray<FString> DialogueLines;
};

// ---------------------------------------------------------------------------------------------

/**
 * @author 유서현
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QUACKTOHELL_API UNPCComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * @author 박시언
	 * @brief NPCComponent의 생성자입니다.
	 */
	UNPCComponent();

protected:
	/**
	 * @author 박시언
	 * @brief 컴포넌트가 시작될 때 호출됩니다.
	 */
	virtual void BeginPlay() override;

	bool LoadPrompt();

	virtual void PerformNPCLogic();

	FString GetPlayerIDAsString() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FString NPCName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FString NPCID;

	FString NPCPersonality;

	TMap<FString, FString> ResponseCache;

	UPROPERTY(Replicated)
	FDateTime LastConversationTime;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/**
	 * @author 박시언
	 * @brief OpenAI 요청이 진행 중인지 여부를 체크합니다. true가 요청 중 false는 요청 없는 상태.
	 */
	UPROPERTY()
	bool bIsRequestInProgress = false;

public:
	/**
	 * @author 박시언
	 * @brief OpenAI 요청을 보낼 수 있는 상태인지 확인하는 함수입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	bool CanSendOpenAIRequest() const;

	/**
	 * @author 박시언
	 * @brief NPC 응답을 브로드캐스트하는 델리게이트입니다.
	 */
	UPROPERTY(BlueprintAssignable, Category = "NPC")
	FOnNPCResponseReceived OnNPCResponseReceived;

	/**
	 * @author 박시언
	 * @brief 플레이어가 NPC와 대화를 시작할 때 호출됩니다.
	 *
	 * @param PlayerInput 플레이어의 입력 대사
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	virtual void StartConversation(const FOpenAIRequest& Request);

	/**
	 * @author 박시언
	 * @brief NPC 간 대화를 시작하는 함수입니다. (최대 4턴)
	 *
	 * @param FirstNPCID 첫 번째로 말을 시작하는 NPC의 ID입니다.
	 * @param SecondNPCID 대화를 받는 NPC의 ID입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void StartNPCToNPCDialog(const FOpenAIRequest& Request);

	/**
	 * @author 박시언
	 * @brief NPC 간 대화 응답을 처리하는 함수입니다. (재귀 방식)
	 *
	 * @param SpeakerNPCID 현재 대화하는 NPC의 ID입니다.
	 * @param ListenerNPCID 상대 NPC의 ID입니다.
	 * @param ReceivedMessage 상대 NPC가 보낸 대사입니다.
	 * @param RemainingTurns 남은 대화 턴 수입니다.
	 */
	void ContinueNPCToNPCDialog(const FOpenAIRequest& Request);

	/**
	 * @author 박시언
	 * @brief NPC가 혼잣말을 실행하는 함수입니다.
	 */
	void PerformNPCMonologue(const FOpenAIRequest& Request);

	/**
	 * @brief 특정 NPC의 대화 기록을 가져올 수 있습니다.
	 * 
	 * \param NPCID
	 * \return 
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<FString> GetP2NDialogueHistory() const;

protected:
	/**
	 * @author 박시언
	 * @brief OpenAI API 요청 공통 함수
	 * \param Prompt
	 * \param Callback
	 */
	void RequestOpenAIResponse(const FOpenAIRequest& AIRequest, TFunction<void(FOpenAIResponse)> Callback);

	/**
	 * @author 박시언
	 * OpenAI API Key 불러오기
	 */
	FString GetAPIKey();

	/**
	 * @author 박시언
	 * OpenAI 응답 JSON 파싱
	 */
	FOpenAIResponse ParseAIResponse(FString ResponseContent);

	/**
	 * @author 박시언
	 * @brief NPC 성격 Get/Set
	 */
	FString GetNPCPersonality() const;
	void SetNPCPersonality(const FString& NewPersonality);

	/**
	 * @author 박시언
	 * @brief NPC 마지막 AI 응답 Get/Set
	 */
	FString GetLastAIResponse() const;
	void SetLastAIResponse(const FString& AIResponse);

	/**
	 * @author 박시언
	 * @brief NPC의 고유 프롬프트 파일 경로 (God이 생성한 JSON 파일의 경로)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FString PromptFilePath;

	/**
	 * @author 박시언
	 * @brief 로드된 프롬프트 내용을 저장하는 변수
	 */
	FString PromptContent;

	/**
	 * @author 박시언
	 * @brief NPCID에 맞는 프롬프트 로드
	 */
	FString LoadPromptForNPC(const FString& InputNPCID) const;

public:
	/**
	 * @author 박시언
	 * @brief NPC의 이름을 반환하는 함수입니다.
	 * @return NPC의 이름 (JSON에서 불러온 값)
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	FString GetNPCName() const;

	/**
	 * @author 박시언
	 * @brief NPC의 ID를 int32로 변환하여 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	int32 GetNPCID() const;

	UFUNCTION(BlueprintCallable, Category = "NPC")
	bool GetIsFirstConversation() const;
	
	UFUNCTION(BlueprintCallable, Category = "NPC")
	bool GetIsRequestInProgress() const;

	UFUNCTION(BlueprintCallable, Category = "NPC")
	FDateTime GetLastConversationTime() const;

	UFUNCTION(BlueprintCallable, Category = "NPC")
	void SetLastConversationTime(FDateTime NewLastConversationTime);


private:
	/**
	 * @author 박시언
	 * @brief 첫 대화 여부를 체크하는 변수 (true면 첫 대화)
	 */
	UPROPERTY()
	bool bIsFirstConversation = true;

	/**
	 * @author 박시언
	 * @brief OpenAI API를 통해 플레이어 입력에 대한 NPC 응답을 요청합니다.
	 *
	 * @param PlayerInput 플레이어의 입력 대사
	 */
	void RequestAIResponse(const FString& PlayerInput);

public:
	/**
	 * @author 박시언
	 * @brief NPC의 응답을 서버로 보내는 RPC 함수입니다.
	 *
	 * 서버는 이 응답을 받아서 UI 또는 클라이언트로 전달할 수 있습니다.
	 *
	 * @param NPCResponse OpenAI로부터 생성된 NPC의 대사
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void SendNPCResponseToServer(const FOpenAIResponse& AIResponse);

protected:
	/**
	 * @author 박시언
	 * @brief SendNPCResponseToServer의 실제 구현입니다.
	 *
	 * @param NPCResponse OpenAI로부터 생성된 NPC의 대사
	 */
	void SendNPCResponseToServer_Implementation(const FOpenAIResponse& AIResponse);

	/**
	 * @author 박시언
	 * @brief SendNPCResponseToServer의 유효성 검증 함수입니다.
	 *
	 * @param NPCResponse 전송될 NPC 대사
	 * @return true면 유효한 데이터로 간주
	 */
	bool SendNPCResponseToServer_Validate(const FOpenAIResponse& AIResponse);

public:
	/**
	 * @author 박시언
	 * @brief NPC 별로 P2N 대화 기록을 저장합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SaveP2NDialogue(const FOpenAIRequest& Request, const FOpenAIResponse& Response);

	/**
	 * @author 박시언
	 * @brief NPC가 플레이어와 대화한 대사를 저장하는 함수입니다.
	 *
	 * @param NPCResponse NPC가 플레이어에게 한 대사입니다.
	 */
	void SaveNPCDialogue(const FString& NPCResponse);

	/**
	 * @brief  대화 기록 저장
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
	TMap<FString, FDialogueHistory> P2NDialogueHistory;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	// Server RPC 함수
	/** @brief 서버에게 NPC의 시작멘트를 요청한다. ServerRPC 내부에서 ClientRPC를 호출. 클라이언트는 ClientRPC 내부에서 응답 멘트를 저장 */
	/*
	UFUNCTION()
	void GetNPCResponseServer(FOpenAIRequest Request); */

	/** @brief 서버에게 플레이어 입력에 대한 NPC의 응답을 요청한다. ServerRPC 내부에서 ClientRPC를 호출. 클라이언트는 ClientRPC 내부에서 응답 멘트를 저장*/
/*
	UFUNCTION(Server, Reliable)
	void ServerRPCGetNPCResponseP2N(FOpenAIRequest Request); */
	

public:
	// 공용 인터페이스
	/** @brief */
	UFUNCTION(Server, Reliable)
	void ServerRPCGetNPCResponse(FOpenAIRequest Request);

};

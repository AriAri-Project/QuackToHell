// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Delegates/DelegateCombinations.h"
#include "NPCComponent.generated.h"

/**
<<<<<<< HEAD
=======
 * @auther 박시언
 * @brief OpenAI API 요청을 위한 구조체
 */
USTRUCT()
struct FOpenAIRequest
{
	GENERATED_BODY()

	FString Prompt;
	int32 MaxTokens = 100;

	// ToJson()은 public임
	FString ToJson() const
	{
		return FString::Printf(TEXT("{ \"prompt\": \"%s\", \"max_tokens\": %d }"), *Prompt, MaxTokens);
	}
};


/**
 * @auther 박시언
 * @brief OpenAI API 응답을 위한 구조체
 */
USTRUCT()
struct FOpenAIResponse
{
	GENERATED_BODY()

	FString ResponseText;

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
		}

		if (Response.ResponseText.IsEmpty())
		{
			Response.ResponseText = TEXT("죄송합니다, 답변할 수 없습니다.");
		}

		return Response;
	}
};

// 델리게이트 선언 추가 (NPC 응답이 발생할 때 실행됨)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNPCResponseReceived, const FString&, NPCResponse);

/**
>>>>>>> origin/SCRUM-433_A_NPCConversation
 * @author 유서현
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QUACKTOHELL_API UNPCComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	FString NPCPersonality;
	TArray<FString> NPCDialogueHistory;
	FString LastAIResponse;

public:
	/**
	 * @auther 박시언
	 * @brief NPCComponent의 생성자입니다.
	 */
	UNPCComponent();

protected:
	/**
	 * @auther 박시언
	 * @brief 컴포넌트가 시작될 때 호출됩니다.
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * @auther 박시언
	 * @brief NPC의 논리 실행 함수 (자식 클래스에서 오버라이드 가능)
	 */
	virtual void PerformNPCLogic();

	// OpenAI 응답이 완료되었을 때 실행될 델리게이트 선언
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnNPCResponseReceived OnNPCResponseReceived;

	/**
	 * @auther 박시언
	 * @brief 플레이어가 NPC와 대화를 시작할 때 호출됩니다.
	 *
	 * @param PlayerInput 플레이어의 입력 대사
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void StartConversation(const FString& PlayerInput);

	/**
	 * @auther 박시언
	 * @brief OpenAI API 요청 공통 함수
	 * \param Prompt
	 * \param Callback
	 */
	void RequestOpenAIResponse(const FOpenAIRequest& AIRequest, TFunction<void(FOpenAIResponse)> Callback);

	/**
	 * @auther 박시언
	 * @brief NPC 성격 Get/Set
	 */
	FString GetNPCPersonality() const;
	void SetNPCPersonality(const FString& NewPersonality);

	/**
	 * @auther 박시언
	 * @brief NPC 대화 기록 Get/Set
	 */
	const TArray<FString>& GetNPCDialogueHistory() const;
	void AddToDialogueHistory(const FString& Dialogue);

	/**
	 * @auther 박시언
	 * @brief NPC 마지막 AI 응답 Get/Set
	 */
	FString GetLastAIResponse() const;
	void SetLastAIResponse(const FString& AIResponse);

protected:
	/**
	 * @auther 박시언
	 * @brief NPC의 고유 프롬프트 파일 경로 (God이 생성한 JSON 파일의 경로)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FString PromptFilePath;

public:
	/**
	 * @auther 박시언
	 * @brief NPC의 이름을 반환하는 함수입니다.
	 * @return NPC의 이름 (JSON에서 불러온 값)
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	FString GetNPCName() const;

protected:
	/**
	 * @auther 박시언
	 * OpenAI API Key 불러오기
	 */
	FString GetAPIKey();

	/**
	 * @auther 박시언
	 * OpenAI 응답 JSON 파싱
	 */
	FOpenAIResponse ParseAIResponse(FString ResponseContent);

	/** @brief NPC의 마지막 대사 */
	FString LastDialogue;

private:
	/**
	 * @auther 박시언
	 * @brief 첫 대화 여부를 체크하는 변수 (true면 첫 대화)
	 */
	UPROPERTY()
	bool bIsFirstConversation = true;

	/**
	 * @auther 박시언
	 * @brief 로드된 프롬프트 내용을 저장하는 변수
	 */
	FString PromptContent;

	/**
	 * @auther 박시언
	 * @brief NPC의 고유 프롬프트 파일을 로드하는 함수
	 */
	void LoadPrompt();

	/**
	 * @auther 박시언
	 * @brief OpenAI API를 통해 NPC의 첫 인사말을 요청합니다.
	 */
	void RequestGreetingFromAI();

	/**
	 * @auther 박시언
	 * @brief OpenAI API를 통해 플레이어 입력에 대한 NPC 응답을 요청합니다.
	 *
	 * @param PlayerInput 플레이어의 입력 대사
	 */
	void RequestAIResponse(const FString& PlayerInput);

public:
	/**
	 * @auther 박시언
	 * @brief NPC 간 대화를 시작하는 함수입니다. (최대 4턴)
	 *
	 * @param FirstNPCID 첫 번째로 말을 시작하는 NPC의 ID입니다.
	 * @param SecondNPCID 대화를 받는 NPC의 ID입니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void StartNPCToNPCDialog(const FString& FirstNPCID, const FString& SecondNPCID);

	/**
	 * @auther 박시언
	 * @brief NPC 간 대화 응답을 처리하는 함수입니다. (재귀 방식)
	 *
	 * @param SpeakerNPCID 현재 대화하는 NPC의 ID입니다.
	 * @param ListenerNPCID 상대 NPC의 ID입니다.
	 * @param ReceivedMessage 상대 NPC가 보낸 대사입니다.
	 * @param RemainingTurns 남은 대화 턴 수입니다.
	 */
	void ContinueNPCToNPCDialog(
		const FString& SpeakerNPCID,
		const FString& ListenerNPCID,
		const FString& ReceivedMessage,
		int RemainingTurns
	);

	public:
		/**
		 * @auther 박시언
		 * @brief NPC의 마지막 대사를 반환하는 함수 (NPCController에서 호출 가능)
		 * @return NPC가 마지막으로 생성한 대사 (FString)
		 */
		FString GetLastDialogue() const;

	/**
	 * @auther 박시언
	 * @brief NPC의 응답을 서버로 보내는 RPC 함수입니다.
	 *
	 * 서버는 이 응답을 받아서 UI 또는 클라이언트로 전달할 수 있습니다.
	 *
	 * @param NPCResponse OpenAI로부터 생성된 NPC의 대사
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void SendNPCResponseToServer(const FString& NPCResponse);

protected:
	/**
	 * @auther 박시언
	 * @brief SendNPCResponseToServer의 실제 구현입니다.
	 *
	 * @param NPCResponse OpenAI로부터 생성된 NPC의 대사
	 */
	void SendNPCResponseToServer_Implementation(const FString& NPCResponse);

	/**
	 * @auther 박시언
	 * @brief SendNPCResponseToServer의 유효성 검증 함수입니다.
	 *
	 * @param NPCResponse 전송될 NPC 대사
	 * @return true면 유효한 데이터로 간주
	 */
	bool SendNPCResponseToServer_Validate(const FString& NPCResponse);

	/**
	 * @author 박시언
	 * @brief 클라이언트가 NPC의 응답을 받을 수 있도록 브로드캐스트하는 함수
	 */
	UFUNCTION(Client, Reliable)
	void ClientReceiveNPCResponse(const FString& NPCResponse);

	void ClientReceiveNPCResponse_Implementation(const FString& NPCResponse);

public:

	/**
	 * @auther 박시언
	 * @brief NPC가 플레이어와 대화한 대사를 저장하는 함수입니다.
	 *
	 * @param NPCResponse NPC가 플레이어에게 한 대사입니다.
	 */
	void SaveNPCDialogue(const FString& NPCResponse);

	/**
	 * @auther 박시언
	 * @brief NPC가 혼잣말을 실행하는 함수입니다.
	 */
	void PerformNPCMonologue();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Client RPC 함수
	/** @brief 클라이언트가 NPC의 응답을 받는 함수 (서버에서 호출됨) */
	UFUNCTION(Client, Reliable)
	void ClientRPCReceiveNPCResponse(const FString& PlayerName, const FString& NPCResponse);
	
	// Server RPC 함수
	/** @brief 서버에게 NPC의 시작멘트를 요청한다. ServerRPC 내부에서 ClientRPC를 호출. 클라이언트는 ClientRPC 내부에서 응답 멘트를 저장 */
	UFUNCTION(Server, Reliable)
	void ServerRPCGetGreeting(const FString& NPCID);

	/** @brief 서버에게 플레이어 입력에 대한 NPC의 응답을 요청한다. ServerRPC 내부에서 ClientRPC를 호출. 클라이언트는 ClientRPC 내부에서 응답 멘트를 저장*/
	UFUNCTION(Server, Reliable)
	void ServerRPCGetNPCResponseP2N(const FString& NPCID, const FString& PlayerInput);
// <<<<<<< HEAD
	
// =======

// >>>>>>> origin/SCRUM-433_A_NPCConversation
	/** @brief 서버에게 N2N 대화의 시작멘트를 요청한다. ServerRPC 내부에서 ClientRPC를 호출. 클라이언트는 ClientRPC 내부에서 응답 멘트를 저장*/
	UFUNCTION(Server, Reliable)
	void ServerRPCGetGreetingN2N(const FString& SpeakerNPCID, const FString& ListenerNPCID);

	/** @brief 서버에게 N2N 대화의 NPC 응답을 요청한다. ServerRPC 내부에서 ClientRPC를 호출. 클라이언트는 ClientRPC 내부에서 응답 멘트를 저장 */
	UFUNCTION(Server, Reliable)
	void ServerRPCGetNPCResponseN2N(const FString& SpeakerNPCID, const FString& ListenerNPCID, const FString& NPCInput);

	/** @brief 서버에게 NPC 혼잣말을 생성하도록 요청한다. ServerRPC 내부에서 ClientRPC를 호출. 클라이언트는 ClientRPC 내부에서 응답 멘트를 저장*/
	UFUNCTION(Server, Reliable)
	void ServerRPCGetNPCMonologue(const FString& NPCID);

public:
	// 공용 인터페이스
	/** @brief */
// <<<<<<< HEAD
	FString GetNPCResponse(const FString& SpeakerNPCID, const FString& NPCInput, const FString& ListenerNPCID = TEXT("")); 
// =======
	FString GetNPCResponse(const FString& SpeakerNPCID, const FString& NPCInput, const FString& ListenerNPCID = TEXT(""));
// >>>>>>> origin/SCRUM-433_A_NPCConversation
};

// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GodFunction.generated.h"

/**
 * 
 */
UCLASS()
class QUACKTOHELL_API UGodFunction : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // OpenAI API를 비동기 호출하는 함수
    static void CallOpenAIAsync(const FString& Prompt, TFunction<void(FString)> Callback, int RetryCount = 0);
    
    // OpenAI API를 호출하여 NPC 데이터를 생성하고 JSON 파일로 저장
    UFUNCTION(BlueprintCallable, Category = "God")
    static void GenerateNPCPrompts(UWorld* World);

    // 피고인 프롬프트 생성
    static void GenerateDefendantPrompt(UWorld* World, TFunction<void()> Callback);

    // 배심원 생성
    static void GenerateJuryNPC(UWorld* World, int JuryIndex);

    // 마을 주민 생성
    static void GenerateResidentNPC(UWorld* World, int ResidentIndex);

    // 기존 프롬프트 파일 삭제 기능 추가
    UFUNCTION(BlueprintCallable, Category = "God")
    static void DeleteOldPromptFiles();

    // JSON 파일 저장
    static bool SavePromptToFile(const FString& FileName, const FString& Content);

    // 증거물 아이템 생성
    static void GenerateEvidenceItems(UWorld* World);

	// Dalle 이미지 다운로드
    static void DownloadDalleImage(const FString& Prompt, const FString& SavePath, TFunction<void()> OnComplete);

private:
    // OpenAI API Key 로드 함수 선언
    static FString LoadOpenAIKey();

    // JSON Escape 함수 선언
    static FString EscapeJSON(const FString& Input);

    // NPC 프롬프트 생성을 타이머를 사용하여 실행
    static void GeneratePromptWithDelay(UWorld* World, const FString& FileName, const FString& Prompt, float Delay);

    // JSON 파일을 읽는 함수 (PromptToGod.json & PromptToNPC.json 처리)
    static FString ReadFileContent(const FString& FilePath);

    static FString ExtractValidJson(FString AIResponse);

    static FString CleanUpJson(FString JsonString);
};

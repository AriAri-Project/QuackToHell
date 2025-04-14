// Copyright_Team_AriAri

#include "GodFunction.h"
#include "GodCall.h"
#include "QGameInstanceVillage.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Json.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "TimerManager.h"

// JSON 파일을 읽는 함수
FString UGodFunction::ReadFileContent(const FString& FilePath)
{
    FString Content;
    if (!FFileHelper::LoadFileToString(Content, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to read file: %s"), *FilePath);
    }
    return Content;
}

// OpenAI API Key 로드 함수 추가 (가장 위쪽에 배치)
FString UGodFunction::LoadOpenAIKey()
{
    FString ConfigPath = FPaths::ProjectDir() + TEXT("OpenAIAuth.ini");
    FString FileContent;

    if (FFileHelper::LoadFileToString(FileContent, *ConfigPath))
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully loaded OpenAIAuth.ini"));
        FString ApiKey;
        if (FileContent.Split(TEXT("="), nullptr, &ApiKey))
        {
            return ApiKey.TrimStartAndEnd();
        }
    }

    UE_LOG(LogTemp, Error, TEXT("Failed to load OpenAI API Key from OpenAIAuth.ini"));
    return TEXT("");
}

// JSON Escape 함수 추가 (OpenAI API 관련 함수들 위쪽)
FString UGodFunction::EscapeJSON(const FString& Input)
{
    FString Escaped = Input;
    Escaped.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
    Escaped.ReplaceInline(TEXT("\""), TEXT("\\\""));
    Escaped.ReplaceInline(TEXT("\n"), TEXT("\\n"));
    Escaped.ReplaceInline(TEXT("\r"), TEXT(""));
    Escaped.ReplaceInline(TEXT("\t"), TEXT("\\t"));

    return Escaped;
}


FString UGodFunction::ExtractValidJson(FString AIResponse)
{
    // OpenAI 응답을 JSON 객체로 파싱
    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(AIResponse);

    if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject->HasField("choices"))
    {
        TArray<TSharedPtr<FJsonValue>> Choices = RootObject->GetArrayField("choices");

        if (Choices.Num() > 0 && Choices[0]->AsObject()->HasField("message"))
        {
            FString JsonContent = Choices[0]->AsObject()->GetObjectField("message")->GetStringField("content");

            // Markdown 코드 블록(```json ... ```) 제거
            JsonContent.ReplaceInline(TEXT("```json"), TEXT(""));
            JsonContent.ReplaceInline(TEXT("```"), TEXT(""));
            JsonContent = JsonContent.TrimStartAndEnd();

            UE_LOG(LogTemp, Log, TEXT("Extracted JSON Content: %s"), *JsonContent);
            return JsonContent;
        }
    }

    UE_LOG(LogTemp, Error, TEXT("Failed to extract valid JSON from AI response!"));
    return TEXT("");
}

FString UGodFunction::CleanUpJson(FString JsonString)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        if (JsonObject->HasField("name"))
        {
            FString Name = JsonObject->GetStringField("name");

            // JSON 내에 같은 이름이 여러 번 들어가면 제거
            JsonObject->SetStringField("name", Name);
        }

        FString CleanJson;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&CleanJson);
        FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

        UE_LOG(LogTemp, Log, TEXT("Cleaned JSON: %s"), *CleanJson);
        return CleanJson;
    }

    UE_LOG(LogTemp, Error, TEXT("Failed to clean JSON!"));
    return JsonString;
}

// JSON 파일 저장 함수
bool UGodFunction::SavePromptToFile(const FString& FileName, const FString& Content)
{
    FString PromptFolder = FPaths::ProjectSavedDir() + TEXT("Prompt/");
    FString FilePath = PromptFolder + FileName;

    // ✅ 절대 경로 변환
    FilePath = FPaths::ConvertRelativePathToFull(FilePath);

    // ✅ 파일 강제 삭제 (존재하지 않는 경우에도 체크)
    // DeleteOldPromptFiles();

    // ✅ 기존 파일 삭제 후 저장
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (PlatformFile.FileExists(*FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ 기존 파일 삭제 후 덮어쓰기: %s"), *FilePath);
        PlatformFile.DeleteFile(*FilePath);
    }

    // ✅ 폴더 존재 여부 확인 및 강제 생성
    if (!FPaths::DirectoryExists(PromptFolder))
    {
        UE_LOG(LogTemp, Warning, TEXT("🚨 폴더가 존재하지 않음. 폴더 생성 시도: %s"), *PromptFolder);
        if (!PlatformFile.CreateDirectoryTree(*PromptFolder))
        {
            UE_LOG(LogTemp, Error, TEXT("❌ 폴더 생성 실패: %s"), *PromptFolder);
            return false;
        }
    }

    // 빈 JSON이면 저장하지 않음
    if (Content.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("저장할 데이터가 비어 있음. Skipping file save for %s"), *FileName);
        return false;
    }

    bool bSuccess = FFileHelper::SaveStringToFile(Content, *FilePath);

    if (bSuccess && FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Log, TEXT("프롬프트 저장 완료: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("프롬프트 저장 실패: %s"), *FilePath);

    }

    return bSuccess;
}

// OpenAI API 호출 함수
void UGodFunction::CallOpenAIAsync(const FString& Prompt, TFunction<void(FString)> Callback, int RetryCount)
{
    FString ApiKey = LoadOpenAIKey();
    if (ApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("OpenAI API Key is missing!"));
        Callback(TEXT(""));
        return;
    }

    TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL("https://api.openai.com/v1/chat/completions");
    Request->SetVerb("POST");
    Request->SetHeader("Authorization", "Bearer " + ApiKey);
    Request->SetHeader("Content-Type", "application/json");

    FString PostData = FString::Printf(
        TEXT("{ \"model\": \"gpt-4o\", \"messages\": [{ \"role\": \"user\", \"content\": \"%s\" }], \"max_tokens\": 2048 }"),
        *EscapeJSON(Prompt)
    );

    Request->SetContentAsString(PostData);

    UE_LOG(LogTemp, Log, TEXT("OpenAI API Request Sent (Attempt %d): %s"), RetryCount + 1, *PostData);

    Request->OnProcessRequestComplete().BindLambda([Callback, Prompt, RetryCount](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (bWasSuccessful && Response.IsValid())
            {
                FString AIResponse = Response->GetContentAsString();
                UE_LOG(LogTemp, Log, TEXT("Raw OpenAI Response: %s"), *AIResponse);

                // JSON 추출 및 정리
                FString CleanedJson = UGodFunction::ExtractValidJson(AIResponse);

                if (!CleanedJson.IsEmpty())
                {
                    Callback(CleanedJson);
                }
                // 최대 3번까지 재시도
                else if (RetryCount < 2)
                {
                    UE_LOG(LogTemp, Warning, TEXT("OpenAI 응답이 비어 있음. %d번째 재시도 중..."), RetryCount + 1);
                    FPlatformProcess::Sleep(1.0f);
                    UGodFunction::CallOpenAIAsync(Prompt, Callback, RetryCount + 1);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("OpenAI 응답이 3회 연속 실패. 요청 중단."));
                    Callback(TEXT(""));
                }
            }
            // 실패 시 최대 3번 재시도
            else if (RetryCount < 2)
            {
                UE_LOG(LogTemp, Warning, TEXT("OpenAI 요청 실패. %d번째 재시도 중..."), RetryCount + 1);
                FPlatformProcess::Sleep(1.0f);
                UGodFunction::CallOpenAIAsync(Prompt, Callback, RetryCount + 1);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("OpenAI 요청이 3회 연속 실패. 요청 중단."));
                Callback(TEXT(""));
            }
        });

    Request->ProcessRequest();
}


void UGodFunction::DeleteOldPromptFiles()
{

    static bool bAlreadyDeleted = false;  // ✅ 중복 실행 방지
    if (bAlreadyDeleted)
    {
        UE_LOG(LogTemp, Log, TEXT("🛑 DeleteOldPromptFiles()가 이미 실행되었으므로 재실행 방지"));
        return;
    }

    FString PromptFolder = FPaths::ProjectSavedDir() + TEXT("Prompt/");

    if (!FPaths::DirectoryExists(PromptFolder))
    {
        UE_LOG(LogTemp, Warning, TEXT("Prompt folder does not exist, skipping deletion."));
        return;
    }

    IFileManager& FileManager = IFileManager::Get();
    // 특정 파일 제외한 파일들만 삭제
    TArray<FString> Files;
    FileManager.FindFiles(Files, *PromptFolder, TEXT(".json"));

    for (const FString& File : Files)
    {
        if (File.Contains("PromptToGod"))
        {
            UE_LOG(LogTemp, Log, TEXT("Skipping essential file: %s"), *File);
            continue;
        }

        FString FilePath = PromptFolder + File;
        FilePath = FPaths::ConvertRelativePathToFull(FilePath);

        if (FileManager.Delete(*FilePath))
        {
            UE_LOG(LogTemp, Log, TEXT("Deleted old Prompt file: %s"), *FilePath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to delete file: %s"), *FilePath);
        }
    }
    bAlreadyDeleted = true;  // ✅ 삭제가 한 번만 실행되도록 설정
    if (UQGameInstanceVillage* GameInstance = Cast<UQGameInstanceVillage>(GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World()->GetGameInstance()))
    {
        GameInstance->StartPromptGeneration();
    }
}

void UGodFunction::GeneratePromptWithDelay(UWorld* World, const FString& FileName, const FString& Prompt, float Delay)
{
    if (!World) return;

    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(TimerHandle, [FileName, Prompt]()
        {
            UE_LOG(LogTemp, Log, TEXT("Sending OpenAI API Request for %s"), *FileName);

            CallOpenAIAsync(Prompt, [=](FString AIResponse)
                {
                    // 응답이 있을 경우에만 파일 저장
                    if (!AIResponse.IsEmpty())
                    {
                        if (UGodFunction::SavePromptToFile(FileName, AIResponse))
                        {
                            UE_LOG(LogTemp, Log, TEXT("Saved AI response for %s"), *FileName);
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("Failed to save %s"), *FileName);
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("OpenAI API returned empty response for %s"), *FileName);
                    }
                });

        }, Delay, false);
}

void UGodFunction::GenerateDefendantPrompt(UWorld* World, TFunction<void()> Callback)
{
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateDefendantPrompt - World is nullptr!"));
        return;
    }

    FString DefendantFilePath = FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToDefendant.json");
    FString PromptToGodPath = FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToGod.json");

    // if (FPaths::FileExists(DefendantFilePath))
    // {
    //    UE_LOG(LogTemp, Warning, TEXT("PromptToDefendant.json 이미 존재하지만, 새로 생성하여 덮어쓰기 진행."));
    // }

     // 이미 존재하면 생성하지 않음
     /*if (FPaths::FileExists(DefendantFilePath))
     {
         UE_LOG(LogTemp, Warning, TEXT("PromptToDefendant.json 이미 존재하므로 생성하지 않음."));
         if (Callback) Callback();
         return;
     }*/

     // PromptToGod.json이 없으면 재시도
    if (!FPaths::FileExists(PromptToGodPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("PromptToGod.json이 존재하지 않음. 피고인 프롬프트 생성 대기!"));
        return;
    }

    // 프롬프트 생성 시작
    UE_LOG(LogTemp, Log, TEXT("PromptToDefendant.json 생성 시작!"));

    FString PromptToGod = ReadFileContent(PromptToGodPath);
    FString DefendantPrompt = FString::Printf(
        TEXT("{ \"task\": \"피고인 정보를 생성하세요.\", "
            "\"instructions\": ["
            "\"PromptToGod.json을 바탕으로 피고인(NPC)의 정보를 생성하세요.\", "
            "\"npcid 값을 '2000'으로 설정하세요.\"], "
            "\"references\": { \"PromptToGod\": \"%s\" } }"),
        *EscapeJSON(PromptToGod.Mid(0, 2000))
    );

    FString DefendantFileName = FString::Printf(TEXT("PromptToDefendant.json"));
    UE_LOG(LogTemp, Log, TEXT("Jury JSON 파일명: %s"), *DefendantFileName);

    // OpenAI API 호출
    CallOpenAIAsync(DefendantPrompt, [World, Callback, DefendantFileName](FString DefendantJson)
        {
            if (!World)
            {
                UE_LOG(LogTemp, Error, TEXT("GenerateDefendantPrompt - World is nullptr in Callback!"));
                return;
            }


            FString CleanedJson = UGodFunction::CleanUpJson(DefendantJson);
            if (UGodFunction::SavePromptToFile(DefendantFileName, CleanedJson))
            {
                UE_LOG(LogTemp, Log, TEXT("PromptToDefendant.json 저장 완료!"));
                if (Callback) Callback();
                GenerateNPCPrompts(World);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("PromptToDefendant.json 저장 실패! 다시 시도."));
                GenerateDefendantPrompt(World, nullptr);
            }
        });
}

void UGodFunction::GenerateNPCPrompts(UWorld* World)
{
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateNPCPrompts - World is nullptr!"));
        return;
    }

    FString PromptToDefendantPath = FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToDefendant.json");
    FString PromptToGodPath = FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToGod.json");


    if (!FPaths::FileExists(PromptToDefendantPath) || !FPaths::FileExists(PromptToGodPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("PromptToGod.json 또는 PromptToDefendant.json이 존재하지 않음. NPC 생성 불가."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("PromptToGod.json 및 PromptToDefendant.json 존재 확인됨. NPC 프롬프트 순차적 생성 시작."));
    GenerateJuryNPC(World, 1);
}

// 배심원 NPC 생성 (순차적으로 진행)
// Copyright_Team_AriAri

#include "GodFunction.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Json.h"

void UGodFunction::GenerateJuryNPC(UWorld* World, int JuryIndex)
{
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateJuryNPC - World is nullptr! JuryIndex: %d"), JuryIndex);
        return;
    }

    if (JuryIndex > 3)
    {
        UE_LOG(LogTemp, Log, TEXT("모든 배심원 생성 완료! 주민 생성 시작."));
        GenerateResidentNPC(World, 1);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("GenerateJuryNPC 실행됨 - JuryIndex: %d"), JuryIndex);

    FString PromptToGod = ReadFileContent(FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToGod.json"));
    FString PromptToDefendant = ReadFileContent(FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToDefendant.json"));

    int32 AssignedNPCID = 2000 + JuryIndex;
    FString JuryPrompt = FString::Printf(
        TEXT("{ \"task\": \"배심원%d 정보를 생성하세요.\", "
            "\"instructions\": ["
            "\"PromptToGod.json과 PromptToDefendant.json을 참고하여 배심원(NPC) 한 명의 정보를 생성하세요.\", "
            "\"npcid\"의 값은 %d여야 합니다\", "
            "\"references\": { \"PromptToGod\": \"%s\", \"PromptToDefendant\": \"%s\" } }"),
        JuryIndex, AssignedNPCID, *EscapeJSON(PromptToGod.Mid(0, 2000)), *EscapeJSON(PromptToDefendant.Mid(0, 2000))
    );

    FString JuryFileName = FString::Printf(TEXT("PromptToJury%d.json"), JuryIndex);
    UE_LOG(LogTemp, Log, TEXT("Jury JSON 파일명: %s"), *JuryFileName);

    CallOpenAIAsync(JuryPrompt, [World, JuryIndex, JuryFileName](FString JuryJson)
        {
            UE_LOG(LogTemp, Log, TEXT("OpenAI 응답 도착 - JuryIndex: %d"), JuryIndex);

            if (!World)
            {
                UE_LOG(LogTemp, Error, TEXT("GenerateJuryNPC Callback - World is nullptr! JuryIndex: %d"), JuryIndex);
                return;
            }

            FString CleanJuryJson = UGodFunction::CleanUpJson(JuryJson);
            if (CleanJuryJson.IsEmpty())
            {
                UE_LOG(LogTemp, Error, TEXT("GenerateJuryNPC Callback - Received empty JSON response! JuryIndex: %d"), JuryIndex);
                GenerateJuryNPC(World, JuryIndex + 1);
                return;
            }

            UE_LOG(LogTemp, Log, TEXT("Jury JSON 데이터 저장 시도 - 파일명: %s"), *JuryFileName);

            if (UGodFunction::SavePromptToFile(JuryFileName, CleanJuryJson))
            {
                UE_LOG(LogTemp, Log, TEXT("Successfully saved JuryPrompt file: %s"), *JuryFileName);
                GenerateJuryNPC(World, JuryIndex + 1);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to save JuryPrompt file: %s"), *JuryFileName);
                GenerateJuryNPC(World, JuryIndex + 1);
            }
        });
}


// 마을 주민 NPC 생성 (순차적으로 진행)
void UGodFunction::GenerateResidentNPC(UWorld* World, int ResidentIndex)
{
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateResidentNPC - World is nullptr! ResidentIndex: %d"), ResidentIndex);
        return;
    }

    if (ResidentIndex > 5)
    {
        UE_LOG(LogTemp, Log, TEXT("모든 주민 생성 완료!"));

        // 증거물 아이템 정보 생성 시작
        GenerateEvidenceItems(World);

        return;
    }

    UE_LOG(LogTemp, Log, TEXT("GenerateResidentNPC 실행됨 - ResidentIndex: %d"), ResidentIndex);

    FString PromptToGod = ReadFileContent(FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToGod.json"));
    FString PromptToDefendant = ReadFileContent(FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToDefendant.json"));

    int32 AssignedNPCID = 2003 + ResidentIndex;
    FString ResidentPrompt = FString::Printf(
        TEXT("{ \"task\": \"마을 주민%d 정보를 생성하세요.\", "
            "\"instructions\": ["
            "\"PromptToGod.json과 PromptToDefendant.json을 참고하여 한 명의 마을 주민(NPC) 정보를 생성하세요.\", "
            "\"npcid\"의 값은 %d여야 합니다\", "
            "\"references\": { \"PromptToGod\": \"%s\", \"PromptToDefendant\": \"%s\" } }"),
        ResidentIndex, AssignedNPCID, *EscapeJSON(PromptToGod.Mid(0, 2000)), *EscapeJSON(PromptToDefendant.Mid(0, 2000))
    );

    FString ResidentFileName = FString::Printf(TEXT("PromptToResident%d.json"), ResidentIndex);
    UE_LOG(LogTemp, Log, TEXT("Generating Resident NPC %d"), ResidentIndex);

    CallOpenAIAsync(ResidentPrompt, [World, ResidentIndex, ResidentFileName](FString ResidentJson)
        {
            UE_LOG(LogTemp, Log, TEXT("OpenAI 응답 도착 - ResidentIndex: %d"), ResidentIndex);

            if (!World)
            {
                UE_LOG(LogTemp, Error, TEXT("GenerateResidentNPC Callback - World is nullptr! ResidentIndex: %d"), ResidentIndex);
                return;
            }

            FString CleanResidentJson = UGodFunction::CleanUpJson(ResidentJson);
            if (CleanResidentJson.IsEmpty())
            {
                UE_LOG(LogTemp, Error, TEXT("GenerateResidentNPC Callback - Received empty JSON response! ResidentIndex: %d"), ResidentIndex);
                GenerateResidentNPC(World, ResidentIndex + 1);
                return;
            }

            UE_LOG(LogTemp, Log, TEXT("Resident JSON 데이터 저장 시도 - 파일명: %s"), *ResidentFileName);

            if (UGodFunction::SavePromptToFile(ResidentFileName, CleanResidentJson))
            {
                UE_LOG(LogTemp, Log, TEXT("Successfully saved ResidentPrompt file: %s"), *ResidentFileName);
                GenerateResidentNPC(World, ResidentIndex + 1);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to save ResidentPrompt file: %s"), *ResidentFileName);
                GenerateResidentNPC(World, ResidentIndex + 1);
            }
        });
}

// 증거물 아이템 생성
void UGodFunction::GenerateEvidenceItems(UWorld* World)
{
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("GenerateEvidenceItems - World is nullptr!"));
        return;
    }

    FString DefendantPath = FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToDefendant.json");
    FString PromptToDefendant = ReadFileContent(DefendantPath);
    if (PromptToDefendant.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("PromptToDefendant.json이 비어 있음!"));
        return;
    }

    FString EvidencePrompt = FString::Printf(TEXT(
        "다음은 피고인의 전생 정보입니다:\n%s\n\n"
        "이 인물의 전생과 관련된 증거물 10개를 생성하세요. 각 증거물은 다음 정보를 포함해야 합니다:\n"
        "1. name: 증거물 이름\n"
        "2. description: 이 증거물이 피고인과 어떤 관련이 있는지 설명\n"
        "3. image_prompt: 해당 증거물에 어울리는 이미지 생성용 프롬프트\n"
        "결과는 JSON 배열로 반환하세요."),
        *EscapeJSON(PromptToDefendant.Left(2000))
    );

    CallOpenAIAsync(EvidencePrompt, [World](FString Response)
        {
            if (Response.IsEmpty())
            {
                UE_LOG(LogTemp, Error, TEXT("증거물 응답이 비어 있음!"));
                return;
            }

            TArray<TSharedPtr<FJsonValue>> EvidenceArray;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
            if (!FJsonSerializer::Deserialize(Reader, EvidenceArray) || EvidenceArray.Num() == 0)
            {
                UE_LOG(LogTemp, Error, TEXT("증거물 JSON 배열 파싱 실패 또는 비어 있음!"));
                return;
            }

            UE_LOG(LogTemp, Log, TEXT("증거물 총 %d개 병렬 처리 시작"), EvidenceArray.Num());

            for (int32 Index = 0; Index < EvidenceArray.Num(); ++Index)
            {
                const TSharedPtr<FJsonObject>* ItemObj;
                if (!EvidenceArray[Index]->TryGetObject(ItemObj))
                {
                    UE_LOG(LogTemp, Warning, TEXT("[%d] JSON 오브젝트 파싱 실패, 건너뜀"), Index + 1);
                    continue;
                }

                FString Name, Desc, ImgPrompt;
                bool b1 = ItemObj->Get()->TryGetStringField("name", Name);
                bool b2 = ItemObj->Get()->TryGetStringField("description", Desc);
                bool b3 = ItemObj->Get()->TryGetStringField("image_prompt", ImgPrompt);

                if (!(b1 && b2 && b3))
                {
                    UE_LOG(LogTemp, Warning, TEXT("[%d] 필드 누락, 건너뜀"), Index + 1);
                    continue;
                }

                FString InfoFilePath = FPaths::ProjectSavedDir() + FString::Printf(TEXT("Prompt/ItemInformation%d.json"), Index + 1);
                FString ImageFilePath = FPaths::ProjectSavedDir() + FString::Printf(TEXT("Items/ItemImage%d.png"), Index + 1);

                // JSON 저장
                TSharedPtr<FJsonObject> SaveObject = MakeShareable(new FJsonObject);
                SaveObject->SetStringField("name", Name);
                SaveObject->SetStringField("description", Desc);
                SaveObject->SetStringField("image_prompt", ImgPrompt);

                FString Output;
                TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
                FJsonSerializer::Serialize(SaveObject.ToSharedRef(), Writer);

                UE_LOG(LogTemp, Log, TEXT("[%d] 저장될 JSON 내용: %s"), Index + 1, *Output);

                if (FFileHelper::SaveStringToFile(Output, *InfoFilePath))
                {
                    UE_LOG(LogTemp, Log, TEXT("[%d] %s 저장 완료"), Index + 1, *InfoFilePath);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("[%d] %s 저장 실패"), Index + 1, *InfoFilePath);
                }

                // 이미지 비동기 요청 (병렬)
                DownloadDalleImage(ImgPrompt, ImageFilePath, [Index]()
                    {
                        UE_LOG(LogTemp, Log, TEXT("[%d] 이미지 저장 완료"), Index + 1);
                    });
            }
        });
}

// Dalle 이미지 다운로드
void UGodFunction::DownloadDalleImage(const FString& Prompt, const FString& SavePath, TFunction<void()> OnComplete)
{
    FString ApiKey = LoadOpenAIKey();
    if (ApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("OpenAI API Key가 없습니다!"));
        return;
    }

    // 이미지 생성 요청 구성 (OpenAI Image API)
    FString FinalPrompt = Prompt + TEXT(", isolated object on transparent background, icon style, minimal shadows");

    TSharedPtr<FJsonObject> RequestObject = MakeShareable(new FJsonObject());
    RequestObject->SetStringField("prompt", FinalPrompt);
    RequestObject->SetStringField("model", "dall-e-3");
    RequestObject->SetStringField("size", "1024x1024");
    RequestObject->SetStringField("response_format", "url");  // URL 받아서 다시 다운로드

    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RequestObject.ToSharedRef(), Writer);

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL("https://api.openai.com/v1/images/generations");
    Request->SetVerb("POST");
    Request->SetHeader("Authorization", "Bearer " + ApiKey);
    Request->SetHeader("Content-Type", "application/json");
    Request->SetContentAsString(RequestBody);

    Request->OnProcessRequestComplete().BindLambda([SavePath](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!bWasSuccessful || !Response.IsValid())
            {
                UE_LOG(LogTemp, Error, TEXT("DALL·E 이미지 생성 요청 실패"));
                return;
            }

            FString ResponseContent = Response->GetContentAsString();
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
            if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
            {
                UE_LOG(LogTemp, Error, TEXT("DALL·E 응답 파싱 실패"));
                return;
            }

            const TArray<TSharedPtr<FJsonValue>>* DataArray;
            if (!JsonObject->TryGetArrayField("data", DataArray) || DataArray->Num() == 0)
            {
                UE_LOG(LogTemp, Error, TEXT("DALL·E 응답에 이미지 URL 없음"));
                return;
            }

            FString ImageUrl = (*DataArray)[0]->AsObject()->GetStringField("url");
            UE_LOG(LogTemp, Log, TEXT("이미지 URL: %s"), *ImageUrl);

            // 이미지 다운로드 요청
            TSharedRef<IHttpRequest> DownloadRequest = FHttpModule::Get().CreateRequest();
            DownloadRequest->SetURL(ImageUrl);
            DownloadRequest->SetVerb("GET");

            DownloadRequest->OnProcessRequestComplete().BindLambda([SavePath](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
                {
                    if (!bWasSuccessful || !Response.IsValid())
                    {
                        UE_LOG(LogTemp, Error, TEXT("이미지 다운로드 실패"));
                        return;
                    }

                    // PNG 저장
                    const TArray<uint8>& ImageData = Response->GetContent();
                    if (FFileHelper::SaveArrayToFile(ImageData, *SavePath))
                    {
                        UE_LOG(LogTemp, Log, TEXT("이미지 저장 완료: %s"), *SavePath);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("이미지 저장 실패: %s"), *SavePath);
                    }
                });

            DownloadRequest->ProcessRequest();
        });

    Request->ProcessRequest();
}

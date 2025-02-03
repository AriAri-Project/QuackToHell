// Copyright_Team_AriAri


#include "GodFunction.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

FString UGodFunction::LoadPromptData()
{
    FString ContentPath = FPaths::ProjectContentDir();
    FString PromptFilePath = FPaths::Combine(ContentPath, TEXT("Prompt/PromptToGod.json"));

    FString FileContent;
    if (FFileHelper::LoadFileToString(FileContent, *PromptFilePath))
    {
        return FileContent;
    }

    return TEXT("Error: Unable to load PromptToGod.json");
}

void UGodFunction::GenerateNPCDataFromOpenAI()
{
    FString Prompt = LoadPromptData();
    if (Prompt.Contains(TEXT("Error")))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load PromptToGod.json"));
        return;
    }

    FString ContentPath = FPaths::ProjectContentDir();
    FString ApiKey;

    // OpenAI API 키 불러오기
    GConfig->GetString(TEXT("OpenAI"), TEXT("ApiKey"), ApiKey, GGameIni);
    if (ApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("OpenAI API Key is missing in DefaultGame.ini"));
        return;
    }

    // 역할별 프롬프트 설정
    TMap<FString, FString> RolePrompts;
    RolePrompts.Add("Defendant", "Generate a detailed backstory for a defendant in a supernatural court trial.");
    RolePrompts.Add("Jury", "Create a unique jury member with personality traits, background, and opinion on the case.");
    RolePrompts.Add("Resident", "Generate a town resident who witnessed something unusual related to the case.");

    // 역할별로 API 요청 생성 및 실행
    for (const auto& RolePrompt : RolePrompts)
    {
        FString Role = RolePrompt.Key;
        Prompt = RolePrompt.Value;

        // Defendant(1명), Jury(3명), Resident(5명)
        for (int i = 1; i <= (Role == "Defendant" ? 1 : (Role == "Jury" ? 3 : 5)); i++)
        {
            FString UniqueRoleName = (Role == "Defendant") ? "Defendant" : FString::Printf(TEXT("%s%d"), *Role, i);

            // OpenAI API 요청 생성
            TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
            Request->SetURL("https://api.openai.com/v1/completions");
            Request->SetVerb("POST");
            Request->SetHeader("Authorization", "Bearer " + ApiKey);
            Request->SetHeader("Content-Type", "application/json");

            FString PostData = FString::Printf(TEXT("{ \"prompt\": \"%s\", \"max_tokens\": 1024 }"), *Prompt);
            Request->SetContentAsString(PostData);

            Request->OnProcessRequestComplete().BindLambda([UniqueRoleName, ContentPath](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
                {
                    if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
                    {
                        FString AIResponse = Response->GetContentAsString();
                        FString FilePath = FPaths::Combine(ContentPath, FString::Printf(TEXT("Prompts/JsonOf%s.json"), *UniqueRoleName));
                        FFileHelper::SaveStringToFile(AIResponse, *FilePath);

                        UE_LOG(LogTemp, Log, TEXT("Saved AI response for %s"), *UniqueRoleName);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to generate data for %s. Response Code: %d"), *UniqueRoleName, Response->GetResponseCode());
                    }
                });

            Request->ProcessRequest();
        }
    }
}

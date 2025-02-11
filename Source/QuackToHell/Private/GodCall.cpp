// Copyright_Team_AriAri

#include "GodCall.h"
#include "GodFunction.h"

// Sets default values for this component's properties
UGodCall::UGodCall()
{
    PrimaryComponentTick.bCanEverTick = false; // Tick 비활성화
}

void UGodCall::PostInitProperties()
{
    Super::PostInitProperties();
    UE_LOG(LogTemp, Log, TEXT("UGodCall::PostInitProperties() 실행됨. 프롬프트 생성 시작"));

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("PostInitProperties - World가 NULL!"));
        return;
    }

    World->GetTimerManager().SetTimerForNextTick([World]()
        {
            UGodFunction::GenerateDefendantPrompt(World, [World]()
                {
                    UE_LOG(LogTemp, Log, TEXT("PromptToDefendant.json 생성 완료! 배심원 생성 시작"));
                    UGodFunction::GenerateJuryNPC(World, 1);
                });
        });
}

void UGodCall::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("GodCall::BeginPlay() 실행됨. 프롬프트 생성 여부 확인"));

}

//void UGodCall::EndPlay(const EEndPlayReason::Type EndPlayReason)
//{
//    Super::EndPlay(EndPlayReason);
//    UE_LOG(LogTemp, Log, TEXT("UGodCall::EndPlay() 실행됨. 기존 프롬프트 삭제 중"));
//    UGodFunction::DeleteOldPromptFiles();
//
//    UWorld* World = GetWorld();
//    if (!World)
//    {
//        UE_LOG(LogTemp, Error, TEXT("UGodCall::EndPlay() - World가 NULL입니다."));
//        return;
//    }
//
//    // 3초 후 PromptToDefendant.json 생성 시작
//    FTimerHandle TimerHandle;
//    World->GetTimerManager().SetTimer(TimerHandle, [World]()
//        {
//            UE_LOG(LogTemp, Log, TEXT("⏳ 3초 경과 후 PromptToDefendant.json 생성 시작"));
//            UGodFunction::GenerateDefendantPrompt(World, [World]()
//                {
//                    UE_LOG(LogTemp, Log, TEXT("✅ PromptToDefendant.json 생성 완료! 배심원 생성 시작"));
//                    UGodFunction::GenerateJuryNPC(World, 1);
//                });
//        }, 3.0f, false);
//}

bool UGodCall::StartGodProcess()
{
    UE_LOG(LogTemp, Log, TEXT("StartGodProcess 실행됨."));

    UWorld* World = GetOwner() ? GetOwner()->GetWorld() : nullptr;
    if (GetOwner())
    {
        UE_LOG(LogTemp, Log, TEXT("GodCall's owner is %s"), *GetOwner()->GetName());
    }
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("StartGodProcess() 실패! World를 가져올 수 없습니다."));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("StartGodProcess - World 정상 인식."));

    FString DefendantFilePath = FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToDefendant.json");

    if (!FPaths::FileExists(DefendantFilePath))
    {
        UE_LOG(LogTemp, Log, TEXT("PromptToDefendant.json이 존재하지 않음. 생성 시작."));
        UGodFunction::GenerateDefendantPrompt(World, [World]()
            {
                UE_LOG(LogTemp, Log, TEXT("PromptToDefendant.json 생성 완료! 배심원 프롬프트 생성 시작."));
                UGodFunction::GenerateJuryNPC(World, 1);
            });
    }

    UE_LOG(LogTemp, Log, TEXT("피고인 데이터 생성 시작."));

    UGodFunction::GenerateDefendantPrompt(World, [World]()
        {
            if (!World)
            {
                UE_LOG(LogTemp, Error, TEXT("StartGodProcess Callback - World is nullptr!"));
                return;
            }

            UE_LOG(LogTemp, Log, TEXT("PromptToDefendant.json 생성 완료! 배심원 생성 시작."));

            FString DefendantFilePath = FPaths::ProjectSavedDir() + TEXT("Prompt/PromptToDefendant.json");
            if (FPaths::FileExists(DefendantFilePath))
            {
                UE_LOG(LogTemp, Log, TEXT("피고인 프롬프트 파일 확인됨. 배심원 생성 시작."));
                UGodFunction::GenerateJuryNPC(World, 1);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("PromptToDefendant.json 생성 확인 실패! 배심원 생성 중단."));
            }
        });

    return true;
}

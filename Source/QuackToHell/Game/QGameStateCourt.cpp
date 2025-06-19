// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/QGameStateCourt.h"
#include <Net/UnrealNetwork.h>



void AQGameStateCourt::MultiRPCStartDirection_Implementation(ECourtDirectionType DirectionType)
{
    if (UWorld* World = GetWorld())
    {
        // NetMode 디버깅
        ENetMode NetMode = World->GetNetMode();
        if (GEngine)
        {
            const FString ModeStr = (NetMode == NM_Client) ? TEXT("Client") :
                (NetMode == NM_ListenServer) ? TEXT("ListenServer") :
                (NetMode == NM_DedicatedServer) ? TEXT("DedicatedServer") :
                (NetMode == NM_Standalone) ? TEXT("Standalone") : TEXT("Unknown");
            GEngine->AddOnScreenDebugMessage(
                /*Key*/ -1,
                /*Time*/ 5.0f,
                /*Color*/ FColor::Yellow,
                FString::Printf(TEXT("[CourtState] NetMode=%s"), *ModeStr)
            );
        }

        // 싱글톤 UI 매니저 획득
        AQCourtUIManager* UIManager = AQCourtUIManager::GetInstance(World);

        // UIManager 존재 여부 디버깅
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                /*Key*/ -1,
                /*Time*/ 5.0f,
                /*Color*/ FColor::Green,
                FString::Printf(TEXT("[CourtState] UIManager is %s"),
                    UIManager ? TEXT("Valid") : TEXT("NULL"))
            );
        }

        if (UIManager)
        {
            // 멀티캐스트로 실제 연출 실행
            UIManager->MultiRPCTurnOnDirection(DirectionType);
        }
    }
}


void AQGameStateCourt::MultiRPCStartUI_Implementation(ECourtUIType UIType)
{
    if (UWorld* World = GetWorld())
    {
        // NetMode 디버깅
        ENetMode NetMode = World->GetNetMode();
        if (GEngine)
        {
            const FString ModeStr = (NetMode == NM_Client) ? TEXT("Client") :
                (NetMode == NM_ListenServer) ? TEXT("ListenServer") :
                (NetMode == NM_DedicatedServer) ? TEXT("DedicatedServer") :
                (NetMode == NM_Standalone) ? TEXT("Standalone") : TEXT("Unknown");
            GEngine->AddOnScreenDebugMessage(
                /*Key*/ -1,
                /*Time*/ 5.0f,
                /*Color*/ FColor::Yellow,
                FString::Printf(TEXT("[CourtState] NetMode=%s"), *ModeStr)
            );
        }

        // 싱글톤 UI 매니저 획득
        AQCourtUIManager* UIManager = AQCourtUIManager::GetInstance(World);

        // UIManager 존재 여부 디버깅
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                /*Key*/ -1,
                /*Time*/ 5.0f,
                /*Color*/ FColor::Green,
                FString::Printf(TEXT("[CourtState] UIManager is %s"),
                    UIManager ? TEXT("Valid") : TEXT("NULL"))
            );
        }

        if (UIManager)
        {
            // 멀티캐스트로 실제 연출 실행
            UIManager->MultiRPCTurnOnUI(UIType);
        }
    }
}

AQGameStateCourt::AQGameStateCourt()
    :Super()
{
    bReplicates = true;

    OpeningStatements.SetNum(2);
    OpeningStatements[0].bServer = true;
    OpeningStatements[1].bServer = false;
}    

void AQGameStateCourt::BeginPlay()
{
    Super::BeginPlay();
}

 TArray<FStatementEntry>& AQGameStateCourt::GetOpeningStatements() 
{
    // TODO: 여기에 return 문을 삽입합니다.
    return OpeningStatements;
}

void AQGameStateCourt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AQGameStateCourt, OpeningStatements);
}



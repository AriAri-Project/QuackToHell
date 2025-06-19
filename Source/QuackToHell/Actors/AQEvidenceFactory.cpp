// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/AQEvidenceFactory.h"
#include "Actors/AQEvidenceActor.h"
#include "Game/QGameInstance.h"
#include "Engine/World.h"
#include "QLogCategories.h"
// Sets default values
AAQEvidenceFactory::AAQEvidenceFactory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AAQEvidenceFactory::BeginPlay()
{
	Super::BeginPlay();
	
	//1. GameInstance에서 리스트 가져오기
	UQGameInstance* GameInstance = Cast<UQGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AAQEvidenceFactory::SpawnAndInitializeEvidenceActors - GameInstance is null"));
		return;
	}
	else {
		GameInstance->OnEvidenceJsonGenerated.AddUObject(this, &AAQEvidenceFactory::SpawnAndInitializeEvidenceActors);
	}
}


void AAQEvidenceFactory::SpawnAndInitializeEvidenceActors()
{
	if (bGenerated) {
		return;
	}
	bGenerated = true;
	if (!HasAuthority())
	{
		UE_LOG(LogLogic, Warning, TEXT("EvidenceFactory: 클라이언트에서 스폰 로직을 호출했음. 서버만 실행해야 합니다."));
		return;
	}
	
	UQGameInstance* GameInstance = Cast<UQGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AAQEvidenceFactory::SpawnAndInitializeEvidenceActors - GameInstance is null"));
		return;
	}
	int EvidenceID = 4000;
	int _EvidenceCount = GameInstance->GetEvidenceList().GetEvidenceCount();
	
	if (_EvidenceCount > SpawnLocations.Num()) {
		_EvidenceCount = SpawnLocations.Num();
	}
	
	for (int i = 0; i < _EvidenceCount; i++) {
		EvidenceList.Add(GameInstance->GetEvidenceList().GetEvidenceWithID(EvidenceID++));
	}
	
	
	if (_EvidenceCount == 0)
	{
		UE_LOG(LogLogic, Warning, TEXT("AAQEvidenceFactory::SpawnAndInitializeEvidenceActors - No evidence found"));
		return;
	}
	for (int32 i = 0; i < _EvidenceCount; i++) {
		FVector Location = (SpawnLocations.IsValidIndex(i) ? SpawnLocations[i] : GetActorLocation());
		FRotator Rotation =  FRotator::ZeroRotator;
		FActorSpawnParameters Params;
		Params.Owner = this;

		//2. EvidenceActor 생성
		AAQEvidenceActor* Actor = GetWorld()->SpawnActor<AAQEvidenceActor>(
			EvidenceActorClass,
			Location, Rotation, Params);
		
		if (Actor) {
			//3. 데이터 초기화
			Actor->SetEvidenceData(*EvidenceList[i]);
		}
		else {
			UE_LOG(LogLogic, Error, TEXT("EvidenceFactory: [%d] 증거물 생성 실패"), i);
		}
	}
}

// Called every frame
void AAQEvidenceFactory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


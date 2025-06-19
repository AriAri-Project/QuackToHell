// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameData/QEvidenceData.h"
#include "AQEvidenceFactory.generated.h"

UCLASS()
class QUACKTOHELL_API AAQEvidenceFactory : public AActor
{
	GENERATED_BODY()
/* -------------------------------------------------------------- */

public:	
	/* 생성자 */
	// Sets default values for this actor's properties
	AAQEvidenceFactory();
	/* 자체 생성 함수 */
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/* 생성할 증거물 오브젝트의 조건들을 설정하는 변수들 */

	/**
	 * @brief 스폰 위치 배열.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence Factory")
	TArray<FVector> SpawnLocations;

	/* 생성할 증거물 오브젝트의 조건들을 설정하는 변수들 */
	/**
	 * @brief 생성할 EvidenceActor의 클래스.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence Factory")
	TSubclassOf<class AAQEvidenceActor> EvidenceActorClass;

/* -------------------------------------------------------------- */

protected:
	/* 자체 생성 함수 */
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	
/* -------------------------------------------------------------- */
private:
	/* 생성할 오브젝트 초기화 관련 함수들 */
	/**
	 * @brief 생성할 오브젝트 스폰 및 초기화 수행.
	 */
	void SpawnAndInitializeEvidenceActors();
	/**
	 * @brief 증거물 데이터들의 리스트. 이 리스트를 각 오브젝트 리스트에 삽입해 오브젝트의 데이터를 초기화해준다.
	 */
	TArray<const FEvidence*> EvidenceList;
private:
	bool bGenerated = false;
};





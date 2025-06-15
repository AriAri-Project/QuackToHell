// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameData/QEvidenceData.h"
#include "AQEvidenceActor.generated.h"

/**
 * @author 전유진.
 * @brief 증거물 오브젝트의 클래스입니다. 이 클래스를 토대로 각 증거물 오브젝트 객체가 생성됩니다.
 * @details AQEvidenceFactory 에 의해 객체가 생성됩니다. 구체형태로 맵에 표시됩니다.
 */
UCLASS()
class QUACKTOHELL_API AAQEvidenceActor : public AActor
{
	GENERATED_BODY()
/* -------------------------------------------------------------- */
public:
	/* 생성자 */
	// Sets default values for this actor's properties
	AAQEvidenceActor();
	/* 자체 함수 */
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/* 증거물데이터 관련함수 */
	/**
	 * @brief 증거물 데이터 설정 함수.
	 * @param NewEvidenceData 설정할 증거물 데이터.
	 */
	void SetEvidenceData(const FEvidence* NewEvidenceData) { EvidenceData = NewEvidenceData; }
	/**
	 * @brief 증거물 데이터 가져오기 함수.
	 * @return 현재 설정된 증거물 데이터.
	 */
	const FEvidence* GetEvidenceData() const;
/* -------------------------------------------------------------- */

protected:
	/* 자체 함수 */
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/* 외관 관련 변수들 */
	/**
	 * @brief 이 오브젝트는 동그란 외관을 갖습니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	UStaticMeshComponent* SphereMeshComponent;

/* -------------------------------------------------------------- */

/* -------------------------------------------------------------- */
/* 증거물 데이터 */
private:
	/* 사용할 클래스 선언 */
	/**
	 * @brief 증거물 데이터 속성값.
	 */
	const FEvidence* EvidenceData;
};

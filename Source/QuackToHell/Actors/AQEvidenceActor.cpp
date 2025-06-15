// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AQEvidenceActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
// Sets default values
AAQEvidenceActor::AAQEvidenceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// ** Enable network replication **
	bReplicates = true;
	SetReplicateMovement(true);

	//루트 컴포넌트를 StaticMeshComponent로 설정
	SphereMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EvidenceSphere"));
	RootComponent = SphereMeshComponent;

	//엔진 기본 Sphere메시 할당
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		SphereMeshComponent->SetStaticMesh(SphereMeshAsset.Object);
		SphereMeshComponent->SetRelativeScale3D(FVector(0.5f)); // 크기 조정
	}

	//물리, 충돌 설정
	SphereMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

const FEvidence* AAQEvidenceActor::GetEvidenceData() const
{
	return EvidenceData;
}

// Called when the game starts or when spawned
void AAQEvidenceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAQEvidenceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AQEvidenceActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include <Net/UnrealNetwork.h>
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
	// Engine의 기본 머티리얼 로드
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SphereMatAsset(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	if (SphereMeshAsset.Succeeded())
	{
		// 메시 세팅
		SphereMeshComponent->SetStaticMesh(SphereMeshAsset.Object);

		// 스케일 조정
		SphereMeshComponent->SetRelativeScale3D(FVector(1.5f));

		// 머티리얼이 있으면 세팅
		if (SphereMatAsset.Succeeded())
		{
			SphereMeshComponent->SetMaterial(0, SphereMatAsset.Object);
		}
	}

	//물리, 충돌 설정
	SphereMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

FEvidence AAQEvidenceActor::GetEvidenceData() const
{
	return EvidenceData;
}

void AAQEvidenceActor::OnRep_EvidenceData()
{
	UE_LOG(LogTemp, Log, TEXT("EvidenceData replicated on client: %s"), *EvidenceData.GetName());
}

// Called when the game starts or when spawned
void AAQEvidenceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAQEvidenceActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAQEvidenceActor, EvidenceData);
}

// Called every frame
void AAQEvidenceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


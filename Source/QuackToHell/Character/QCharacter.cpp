// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QCharacter.h"

#include "EngineUtils.h"
#include "QLogCategories.h"
#include "Blueprint/UserWidget.h"
#include "UObject/SoftObjectPath.h"
#include "UI/QNameWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AQCharacter::AQCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*이름표 UI 컴포넌트*/
	this->NameWidgetComponent= CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));
	EWidgetSpace WidgetSpace = EWidgetSpace::Screen;
	this->NameWidgetComponent->SetWidgetSpace(WidgetSpace);
	this->NameWidgetComponent->SetDrawAtDesiredSize(true);
	this->NameWidgetComponent->SetupAttachment(RootComponent);
	TSubclassOf<UQNameWidget> _NameWidget;
	//QNameWidget을 상속한 클래스만 담을 수 있도록 강제한다.
	this->NameWidgetComponent->SetWidgetClass(_NameWidget);
	/*멀티플레이 관련*/
	bReplicates = true;
}

// Called when the game starts or when spawned
void AQCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (NameWidgetComponent)
	{
		UUserWidget* UserWidget = NameWidgetComponent->GetWidget();
		if (UserWidget)
		{
			NameWidget = Cast<UQNameWidget>(UserWidget);
		}
	}

	// 서버에서 실행 -> 오너십을 부여하기 위해
	if (HasAuthority())
	{
		APlayerController* LocalPlayerController = nullptr;

		// GetWorld()에서 모든 플레이어 컨트롤러를 순회하며, 로컬 플레이어 컨트롤러 찾기
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = Cast<APlayerController>(*It);
			if (PC && PC->IsLocalPlayerController()) // 로컬 플레이어인지 확인
			{
				LocalPlayerController = PC;
				break;
			}
		}

		if (LocalPlayerController)
		{
			SetOwner(LocalPlayerController);
			UE_LOG(LogLogic, Log, TEXT("%s 's Owner is %s"), *this->GetName(), *GetOwner()->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find Local Player Controller!"));
		}
	}

}

const FString& AQCharacter::GetCharacterName() const
{
	return CharacterName;	
}

void AQCharacter::SetCharacterName(FString& Name)
{
	CharacterName = Name;
}

TObjectPtr<class UQNameWidget> AQCharacter::GetNameWidget() const
{
	return NameWidget;
}



// Called every frame
void AQCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AQCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

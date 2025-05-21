// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "QCharacter.generated.h"

UCLASS()
class QUACKTOHELL_API AQCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQCharacter(const FObjectInitializer& ObjectInitializer);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	/**
	 * @brief 이름 리턴 함수
	 * @return 이름
	 */
	const FString& GetCharacterName() const;
	/**
	 * @brief 이름 설정 함수
	 * @return 이름
	 */
	void SetCharacterName(FString& Name);

	/**
	 * @brief name UI 컴포넌트입니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent> NameWidgetComponent;

	/**
	 * @brief 네임위젯을 리턴합니다..
	 * 
	 * @return namewidget 
	 */
	TObjectPtr<class UQNameWidget> GetNameWidget() const;

private:
	/** @brief 네임위젯 입니다. */
	UPROPERTY()
	TObjectPtr<class UQNameWidget> NameWidget;
	
	/**
	 * @brief 이름
	 */
	FString CharacterName;
};

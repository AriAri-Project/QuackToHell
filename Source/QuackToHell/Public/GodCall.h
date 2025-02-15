// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GodFunction.h"
#include "GodCall.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUACKTOHELL_API UGodCall : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGodCall();

protected:
	virtual void PostInitProperties() override;

	virtual void BeginPlay() override;

	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// GodFunction 호출
	UFUNCTION(BlueprintCallable, Category = "God")
	bool StartGodProcess();

};

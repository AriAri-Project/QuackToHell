// OpenAI, Copyright LifeEXE. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "Provider/Types/BatchTypes.h"
#include "Provider/Types/CommonTypes.h"
#include "CancelBatchAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnCancelBatch,
    const FCancelBatchResponse&, Response,
    const FOpenAIResponseMetadata&, ResponseMetadata,
    const FOpenAIError&, RawError
);

class UOpenAIProvider;

UCLASS()
class UCancelBatchAction : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCancelBatch OnCompleted;

    virtual void Activate() override;

private:
    /**
     * @param URLOverride Allows for the specification of a custom endpoint. This is beneficial when using a proxy.
     * If this functionality is not required, this parameter can be left blank.
     */
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "OpenAI | Batches")
    static UCancelBatchAction* CancelBatch(const FString& BatchID, const FOpenAIAuth& Auth, const FString& URLOverride);

    void TryToOverrideURL(UOpenAIProvider* Provider);

    void OnCancelBatchCompleted(const FCancelBatchResponse& Response, const FOpenAIResponseMetadata& ResponseMetadata);
    void OnRequestError(const FString& URL, const FString& Content);

private:
    FString BatchID;
    FOpenAIAuth Auth;
    FString URLOverride{};
};

// Copyright_Team_AriAri

#pragma once

#include "CoreMinimal.h"
#include "Character/QCharacter.h"
#include "QNPC.generated.h"

/**
 * @author 전유진 유서현
 * @brief NPC 캐릭터 클래스입니다.
 */
UCLASS()
class QUACKTOHELL_API AQNPC : public AQCharacter
{
	GENERATED_BODY()

public:
	AQNPC();

private:
	// NPC 대화
	UPROPERTY(Replicated)
	bool bCanStartConversN2N = false;

	UPROPERTY(Replicated)
	bool bCanFinishConversN2N = false;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Reliable)
	void ServerRPCCanCanStartConversN2N(const AQNPC* NPC);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCCanCanFinishConversN2N(const AQNPC* NPC);

public:
	// 공용 인터페이스
	/** @brief N2N 대화가 가능한지에 대한 Getter*/
	bool GetCanStartConversN2N(const AQNPC* NPC);
	/** @brief NPC와의 대화를 마칠 수 있는지에 대한 Getter*/
	bool GetCanFinishConversN2N(const AQNPC* NPC);
};

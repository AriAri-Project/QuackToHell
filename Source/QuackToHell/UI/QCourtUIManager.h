// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "QCourtUIManager.generated.h"
UENUM()
/** @brief 법원 내 존재해야하는 UI목록들 */
enum class ECourtUIType :uint8 {
	OpeningStatement, //모두진술 UI
	EvidenceExam,//증거조사UI
	ExamDefendant, //피고인심문 UI
	CourtTimer, // 법원 타이머
	InputBox, //입력창
};
UENUM()
/** @brief 법원 내 존재해야하는 연출 목록들 */
enum class ECourtDirectionType :uint8 {
	Opening,//도입부
	OpeningStatement,//모두진술
	EvidenceExamStarting,//증거조사
	ExamDefendantStarting,//피고인심문 시작
	ExamDefendantResponse,//피고인심문 답변
};


/**
 * @author 전유진.
 * @brief 법원 UI 매니저입니다. 키고끄는 책임이 있습니다. 
 * 흐름 덩어리에 대해서도 키고 끌 수 있습니다. (ex. 모두진술, 증거조사 ..  )
 * @details 법원 내 UI 오브젝트 정보를 가지고 있습니다.
 */
UCLASS()
class QUACKTOHELL_API AQCourtUIManager : public AActor
{
	GENERATED_BODY()
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	/*연출켜기*/
	/**
	 * @brief 재판 도입부 연출을 틉니다.
	 * 
	 */
	UFUNCTION(NetMulticast, Reliable)
	void TurnOnOpening();
	/**
	 * @brief 모두진술 연출을 틉니다.
	 *
	 */
	UFUNCTION(NetMulticast, Reliable)
	void TurnOnOpeningStatement();

public:
	/*깔끔 ver: 연출켜기*/
	/**
	 * @brief 연출켜기
	 * @param 연출타입
	 */
	UFUNCTION(NetMulticast, Reliable)
	void TurnOnDirection(ECourtDirectionType DirectionType);
public: 
	/*UI마무리*/
	/**
	 * @brief UI 마무리 : 입력마무리 처리, 타이머 마무리 초기화, .. 
	 * @param 현재 켜져있는 UI  (마무리하려는 UI)
	 */
	UFUNCTION(NetMulticast, Reliable)
	void FinishUIWork(ECourtUIType UIType);
public:
	/**
	 * @brief UI를 켭니다.
	 *
	 * @param UIType
	 */
	void TurnOnUI(ECourtUIType UIType);
	/**
	 * @brief UI를 끕니다.
	 *
	 * @param UIType
	 */
	void TurnOffUI(ECourtUIType UIType);
	/**
	 * @brief 싱글톤 인스턴스 반환.
	 * @return 싱글톤 인스턴스
	 */
	static TObjectPtr<AQCourtUIManager> GetInstance(TObjectPtr<UWorld> World);
	/**
	 * @brief 재판장의 위젯들을 리턴합니다..
	 *
	 * @return 재판장 위젯들의 map
	 */
	TMap<ECourtUIType, TObjectPtr<UUserWidget>> GetActivedCourtWidgets() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Called when level is unloded
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** @brief 싱글톤 인스턴스 */
	static TObjectPtr<AQCourtUIManager> Instance;
	AQCourtUIManager();
	/**
	 * @brief 재판장 내 존재해야하는 위젯클래스들: UI타입과 위젯클래스 매핑.
	 * UIWidgetClasses에 따라서 UI를 생성한다: 해당 UI를 키라는 입력이 들어올 때
	 * 예외: 기본적으로 화면에 있어야 하는 건 초기화 때 생성& visible처리
	 */
	TMap<ECourtUIType, TSubclassOf<UUserWidget>> UIWidgetClasses;
	/** @brief 재판장 위젯 관리 */
	TMap<ECourtUIType, TObjectPtr<UUserWidget>> ActivedCourtWidgets;
	/**
	 * @brief
	 * 맵 로드 시 실행.
	 *
	 */
	void OnMapLoad();
	/**
	 * @brief.재판장맵인지 체크 (마을맵이 아닐 시 싱글톤생성 x)
	 */
	bool IsCourtMap();
};

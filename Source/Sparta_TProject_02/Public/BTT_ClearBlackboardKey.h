// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ClearBlackboardKey.generated.h"

/*
* 에디터에서 지정한 블랙보드 키(Blackboard Key)의 값을 지우는(Clear) 간단한 유틸리티 태스크
*/
UCLASS()
class SPARTA_TPROJECT_02_API UBTT_ClearBlackboardKey : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_ClearBlackboardKey();

protected:
	// 태스크가 실행될때 호출되어 키의 값을 지우는 핵심 함수(Core function)
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	// 값을 지울 대상 블랙보드 키를 지정, 에디터에서 설정
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector KeyToClear;



};

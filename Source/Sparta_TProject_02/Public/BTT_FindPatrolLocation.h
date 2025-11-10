// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FindPatrolLocation.generated.h"

/*
* AI의 현재 위치를 기준으로 순찰 반경(Patrol Radius) 내에서
* 이동 가능한(reachable) 임의의 위치를 찾아 블랙보드에 저장
*/
UCLASS()
class SPARTA_TPROJECT_02_API UBTT_FindPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_FindPatrolLocation();

protected:
	// 순찰 지점을 찾는 핵심 함수(Core Function)
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// 순찰 반경. 에디터 수정 가능
	UPROPERTY(EditAnywhere, Category = "AI")
	float PatrolRadius = 500.0f;

	// 순찰 지점(FVector)을 블랙보드 키 'PatrolLocationKey'에 저장
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationKey;
};

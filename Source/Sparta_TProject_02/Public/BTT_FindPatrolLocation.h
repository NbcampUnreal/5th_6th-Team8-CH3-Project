// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FindPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class SPARTA_TPROJECT_02_API UBTT_FindPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_FindPatrolLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	float PatrolRadius = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationKey;
};

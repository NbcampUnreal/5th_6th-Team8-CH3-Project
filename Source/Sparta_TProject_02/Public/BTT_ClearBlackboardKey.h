// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ClearBlackboardKey.generated.h"

/**
 * 
 */
UCLASS()
class SPARTA_TPROJECT_02_API UBTT_ClearBlackboardKey : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_ClearBlackboardKey();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector KeyToClear;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};

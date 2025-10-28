// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ClearBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_ClearBlackboardKey::UBTT_ClearBlackboardKey()
{
	NodeName = TEXT("Clear Blackboard KEY");
}

EBTNodeResult::Type UBTT_ClearBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComp->ClearValue(KeyToClear.SelectedKeyName);

	return EBTNodeResult::Succeeded;
}


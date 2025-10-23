// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"
#include "AIController.h"
#include "AIMonsterBase.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"


UBTT_Attack::UBTT_Attack()
{
	NodeName = TEXT("Attack Montage");

	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	MyOwnerComp = &OwnerComp;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AAIMonsterBase* Monster = Cast<AAIMonsterBase>(AIController->GetPawn());
	if (Monster == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (MontageToPlay == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &UBTT_Attack::OnMontageEnded, &OwnerComp);

	AnimInstance->Montage_Play(MontageToPlay);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

	return EBTNodeResult::InProgress;
}

void UBTT_Attack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (bInterrupted)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	}
	else
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTT_Attack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AAIMonsterBase* Monster = Cast<AAIMonsterBase>(AIController->GetPawn());
		if (Monster && Monster->GetMesh() && Monster->GetMesh()->GetAnimInstance())
		{
			Monster->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, MontageToPlay);
		}
	}

	return EBTNodeResult::Aborted;
}
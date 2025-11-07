// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PlayMontage.h" // 헤더 이름 변경
#include "AIC_Monster.h"
#include "AIMonsterBase.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTT_PlayMontage::UBTT_PlayMontage() // 생성자 이름 변경
{
    NodeName = TEXT("Play Montage"); // 노드 이름도 범용적으로 변경
    bCreateNodeInstance = true; // 비동기 태스크는 이 옵션이 필수
}

EBTNodeResult::Type UBTT_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    MyOwnerComp = &OwnerComp;

    AAIC_Monster* AIController = Cast<AAIC_Monster>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    AAIMonsterBase* Monster = Cast<AAIMonsterBase>(AIController->GetPawn());
    if (!Monster) return EBTNodeResult::Failed;

    // 에디터에서 몽타주가 지정되지 않았다면 실패 처리
    if (MontageToPlay == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTT_PlayMontage: MontageToPlay is not set!"));
        return EBTNodeResult::Failed;
    }

    UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
    if (!AnimInstance) return EBTNodeResult::Failed;

    AIController->StartRotatingToTarget();

    FOnMontageEnded MontageEndedDelegate;
    MontageEndedDelegate.BindUObject(this, &UBTT_PlayMontage::OnMontageEnded, &OwnerComp);

    AnimInstance->Montage_Play(MontageToPlay);
    AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

    return EBTNodeResult::InProgress;
}

void UBTT_PlayMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
    AAIC_Monster* AIController = Cast<AAIC_Monster>(OwnerComp->GetAIOwner());
    if (AIController)
    {
        AIController->StopRotatingToTarget();
    }

    if (bInterrupted)
    {
        FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
    }
    else
    {
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type UBTT_PlayMontage::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIC_Monster* AIController = Cast<AAIC_Monster>(OwnerComp.GetAIOwner());
    if (AIController)
    {
        AIController->StopRotatingToTarget();

        AAIMonsterBase* Monster = Cast<AAIMonsterBase>(AIController->GetPawn());
        if (Monster && Monster->GetMesh() && Monster->GetMesh()->GetAnimInstance())
        {
            Monster->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, MontageToPlay);
        }
    }
    return EBTNodeResult::Aborted;
}
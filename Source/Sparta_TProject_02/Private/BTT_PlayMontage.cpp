// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PlayMontage.h"
#include "AIController.h"
#include "AIC_Monster.h"
#include "AIMonsterBase.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTT_PlayMontage::UBTT_PlayMontage()
{
    NodeName = TEXT("Play Montage");
    bCreateNodeInstance = true; // 비동기 태스크는 이 옵션이 필수
}

EBTNodeResult::Type UBTT_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    MyOwnerComp = &OwnerComp;

    // -- 유효성 검사 ---
    // AI 컨트롤러 Get, 유효성 검사
    AAIC_Monster* AIController = Cast<AAIC_Monster>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    // AI 컨트롤러가 조종하는 폰 Get, 유효성 검사
    AAIMonsterBase* Monster = Cast<AAIMonsterBase>(AIController->GetPawn());
    if (!Monster) return EBTNodeResult::Failed;

    // 몽타주 유효성 검사
    if (MontageToPlay == nullptr)
    {
        // UE_LOG(LogTemp, Warning, TEXT("BTT_PlayMontage: MontageToPlay is not set!"));
        return EBTNodeResult::Failed;
    }
    // AnimInstance 유효성 검사
    UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
    if (!AnimInstance) return EBTNodeResult::Failed;

    // -- 몽타주 재생 ---
    // AI 컨트롤러에 회전 시작 요청
    AIController->StartRotatingToTarget();

    // 몽타주 재생이 끝날때 호출되는 델리게이트를 바인딩
    // MontageEndedDelegate 알림이 울리면 OnMontageEnded 함수 호출
    FOnMontageEnded MontageEndedDelegate;
    MontageEndedDelegate.BindUObject(this, &UBTT_PlayMontage::OnMontageEnded, &OwnerComp);

    // 몽타주를 재생하고, 종료 델리게이트 등록
    // MontageToPlay가 종료되면 MontageEndDelegate 알림을 울림
    AnimInstance->Montage_Play(MontageToPlay);
    AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

    // 태스크 결과 'InProgress'로 반환
    return EBTNodeResult::InProgress;
}

// 몽타주 재생이 완료/중단 되었을때 호출
void UBTT_PlayMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
    // AI 컨트롤러에 회전을 멈추라고 요청
    AAIC_Monster* AIController = Cast<AAIC_Monster>(OwnerComp->GetAIOwner());
    if (AIController)
    {
        AIController->StopRotatingToTarget();
    }

    // 몽타주 재생이 중간에 취소되었다면 태스크 '실패' 처리, 완료되었다면 태스크 '성공' 처리
    // FinishLatenTask는 Inprogress 태스크를 종료시키는 함수
    if (bInterrupted)
    {
        FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
    }
    else
    {
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    }
}

// 데코레이터 조건 변화 등 외부 요인에 의해 태스크가 강제로 중단될 때 호출
EBTNodeResult::Type UBTT_PlayMontage::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI 컨트롤러에 회전을 멈추라고 요청
    // 재생중인 몽타주를 델리게이트 대신 Montage_Stop을 이용해 강제로 중단
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
    // 몽타주가 중단되어 태스크 '중단' 처리
    return EBTNodeResult::Aborted;
}
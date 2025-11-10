// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PlayMontage.generated.h"

/*
* 애니메이션 몽타주를 재생하고, 몽타주가 끝날 때 까지 태스크를 진행 중(Inprogress)상태로 유지
* 행동 트리가 애니메이션 몽타주 재생 완료 시점에 진행됨
* 몽타주가 재생되는 동안 AI가 목표물을 향해 회전하도록 함
*/

UCLASS()
class SPARTA_TPROJECT_02_API UBTT_PlayMontage : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTT_PlayMontage();

protected:
    // 애니메이션 몽타주를 실행하는 핵심 함수(Core Function)
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    // 애니메이션 몽타주를 중단하는 핵심 함수(Core Function)
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    // 재생할 애니메이션 몽타주, 에디터에서 수정 가능
    UPROPERTY(EditAnywhere, Category = "AI")
    UAnimMontage* MontageToPlay;

    // 몽타주 재생이 완료/중단 시 호출되는 함수
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);

    // OwnerComp를 넘기기 위한 멤버 변수
    UPROPERTY()
    TObjectPtr<UBehaviorTreeComponent> MyOwnerComp;
};

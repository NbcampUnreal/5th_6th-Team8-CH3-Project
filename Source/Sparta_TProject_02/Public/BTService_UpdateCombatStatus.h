// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdateCombatStatus.generated.h"

/*
* 서비스가 연결된 행동 트리가 활성화 된 동안
* 주기적으로 AI의 전투 상태를 계산하여 블랙보드에 업데이트
*/
UCLASS()
class SPARTA_TPROJECT_02_API UBTService_UpdateCombatStatus : public UBTService_BlackboardBase
{
    GENERATED_BODY()

public:
    UBTService_UpdateCombatStatus();

protected:
    // 상태를 업데이트 하는 핵심 함수(Core Function)
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
    // -- 에디터에서 연결할 상태 키 --
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector DistanceToTargetKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector HasLineOfSightKey;
};
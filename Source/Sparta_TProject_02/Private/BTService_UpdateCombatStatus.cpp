// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateCombatStatus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateCombatStatus::UBTService_UpdateCombatStatus()
{
    NodeName = TEXT("Update Combat Status");

    // 서비스가 실행될 주기(Interval)를 설정(기본값 0.2초)
    Interval = 0.2f;
}

void UBTService_UpdateCombatStatus::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // -- 유효성 검사 --
    // AI 컨트롤러와 블랙보드 컴포넌트를 Get, 유효성 검사
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (AIController == nullptr || BlackboardComp == nullptr)
    {
        return;
    }

    // 블랙보드에서 TargetActor를 가져옵니다.
    // GetValueAsObject는 UObject*를 반환하므로, AACtor*로 캐스팅하여 사용
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (TargetActor == nullptr)
    {
        return;
    }

    // AI 컨트롤러가 조정하는 Pawn Get, 유효성 검사
    APawn* ControlledPawn = AIController->GetPawn();
    if (ControlledPawn == nullptr)
    {
        return;
    }

    // 1. 플레이어와의 거리 계산(float) 및 블랙보드 업데이트
    const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), TargetActor->GetActorLocation());
    BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);

    // 2. 플레이어 시야 확인(bool) 및 블랙보드 업데이트
    const bool bHasLineOfSight = AIController->LineOfSightTo(TargetActor);
    BlackboardComp->SetValueAsBool(HasLineOfSightKey.SelectedKeyName, bHasLineOfSight);
}


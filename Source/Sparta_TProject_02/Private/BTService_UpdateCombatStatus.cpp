// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateCombatStatus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h" // GetPlayerCharacter 사용을 위해

UBTService_UpdateCombatStatus::UBTService_UpdateCombatStatus()
{
    NodeName = TEXT("Update Combat Status");

    // 서비스가 실행될 주기(Interval)를 설정합니다.
    Interval = 0.2f;
}

void UBTService_UpdateCombatStatus::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // AI 컨트롤러와 블랙보드 컴포넌트를 가져옵니다.
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    if (AIController == nullptr || BlackboardComp == nullptr)
    {
        return;
    }

    // 블랙보드에서 TargetActor를 가져옵니다.
    UObject* TargetObject = BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName);
    AActor* TargetActor = Cast<AActor>(TargetObject);

    if (TargetActor == nullptr)
    {
        // 타겟이 없으면 아무것도 하지 않습니다.
        return;
    }

    APawn* ControlledPawn = AIController->GetPawn();
    if (ControlledPawn == nullptr)
    {
        return;
    }

    // 1. 플레이어와의 거리 계산 및 블랙보드 업데이트
    const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), TargetActor->GetActorLocation());
    BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);

    // 2. 플레이어 시야 확인 및 블랙보드 업데이트
    const bool bHasLineOfSight = AIController->LineOfSightTo(TargetActor);
    BlackboardComp->SetValueAsBool(HasLineOfSightKey.SelectedKeyName, bHasLineOfSight);
}


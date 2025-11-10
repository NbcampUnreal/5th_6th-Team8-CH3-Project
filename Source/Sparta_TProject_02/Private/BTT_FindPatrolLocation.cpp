// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindPatrolLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindPatrolLocation::UBTT_FindPatrolLocation()
{
	NodeName = TEXT("Find Patrol Location");
}

// 순찰 지점을 찾는 핵심 함수(Core Function)
EBTNodeResult::Type UBTT_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI 컨트롤러 Get & 유효성 검사
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// AI 컨트롤러가 조종하는 폰 Get & 유효성 검사
	APawn* ControlledPawn = AIController->GetPawn();
	if (ControlledPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 월드의 내비게이션 시스템 Get & 유효성 검사
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 랜덤 위치를 생성 & 순찰 반경 내에 랜덤 위치로 이동 가능한지 확인
	// 확인시 해당 위치를 블랙보드 키에 저장하고 '태스크 성공' 처리
	FNavLocation RandomLocation;
	bool bFound = NavSystem->GetRandomReachablePointInRadius(ControlledPawn->GetActorLocation(), PatrolRadius, RandomLocation);
	if (bFound)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
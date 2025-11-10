// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ClearBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_ClearBlackboardKey::UBTT_ClearBlackboardKey()
{
	NodeName = TEXT("Clear Blackboard KEY");
}

// 핵심 함수
EBTNodeResult::Type UBTT_ClearBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 현재 행동 트리가 사용하는 블랙보는 컴포넌트 Get, 유효성 검사
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 에디터에서 지정한 KeyToClear의 값을 지움
	BlackboardComp->ClearValue(KeyToClear.SelectedKeyName);

	// 작업이 완료되면 '성공'을 반환
	return EBTNodeResult::Succeeded;
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PlayMontage.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API UBTT_PlayMontage : public UBTTaskNode // 클래스 이름 변경
{
    GENERATED_BODY()

public:
    UBTT_PlayMontage(); // 생성자 이름 변경

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    // 이 변수를 에디터에서 직접 지정할 수 있도록 변경합니다.
    UPROPERTY(EditAnywhere, Category = "AI")
    UAnimMontage* MontageToPlay;

    // 몽타주 종료 시 호출될 콜백 함수
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);

    // OwnerComp를 콜백 함수에 넘기기 위해 멤버 변수로 저장
    UPROPERTY()
    TObjectPtr<UBehaviorTreeComponent> MyOwnerComp;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIC_Monster.generated.h"

/**
 * 
 */
UCLASS()
class SPARTA_TPROJECT_02_API AAIC_Monster : public AAIController
{
	GENERATED_BODY()

public:
    AAIC_Monster();

    virtual void Tick(float DeltaSeconds) override;
    
    FRotator AttackStartRotation;

protected:
    virtual void OnPossess(APawn* InPawn) override;

private:
    // Perception Component 선언
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    class UAIPerceptionComponent* AIPerceptionComponent;

    // 시각 감각 설정 객체 선언
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    class UAISenseConfig_Sight* SightConfig;

    // 인지 정보가 업데이트될 때 호출될 함수
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};

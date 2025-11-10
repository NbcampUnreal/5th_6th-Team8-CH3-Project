#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIC_Monster.generated.h"

/**
 * 모든 AI를 제어하는 공통 AI 컨트롤러 클래스
 * AIPerceptionComponent를 사용하여 주변 환경(시각, 청각, 피해)을 인지하고
 * 결과를 블랙보드에 업데이트하여 행동 트리가 의사결정을 내리도록 함
 * 또한, 공격 중 캐릭터의 회전을 제어하는 기능도 담당
 */
UCLASS()
class SPARTA_TPROJECT_02_API AAIC_Monster : public AAIController
{
	GENERATED_BODY()

public:
    AAIC_Monster();

    // --- Overrides ---
    virtual void OnPossess(APawn* InPawn) override;

    virtual void Tick(float DeltaSeconds) override;

    // --- 회전 제어 ---
    void StartRotatingToTarget(); // 캐릭터의 회전을 '컨트롤러 제어 모드'로 전환

    void StopRotatingToTarget(); // 캐릭터의 회전을 '이동 방향 제어 모드'로 전환

private:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    bool bIsRotatingToTarget = false; // 회전 관련 상태 변수

    // --- AI Perception (AI감각) --- 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    class UAIPerceptionComponent* AIPerceptionComponent; // 감각 컴포넌트 선언

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    class UAISenseConfig_Sight* SightConfig; // 시각 감각의 상세 설정을 담는 객체

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus); // 새로운 자극을 감지했을 때 호출되는 이벤트 함수 


};

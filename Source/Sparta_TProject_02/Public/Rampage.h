#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "AIMonsterBase.h"
#include "Rampage.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API ARampage : public AAIMonsterBase
{
	GENERATED_BODY()

public:
	ARampage();

protected:
	virtual void BeginPlay() override;

public:
	/** Rampage의 광역 공격(Smash) 데미지 판정을 수행합니다. */
	UFUNCTION(BlueprintCallable, Category = "Rampage Skills")
	void PerformSmashAttack();

	/** Rampage의 분노(Energize) 버프 효과를 적용합니다. */
	UFUNCTION(BlueprintCallable, Category = "Rampage Skills")
	void Energize();

	virtual void Die() override;

protected:
	/** 분노 버프 효과를 종료합니다. */
	void EndEnergize();

private:
	// 머티리얼 인스턴스를 저장할 포인터 배열
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterialInstances;

	// 색깔 머티리얼에서 사용할 파라미터의 이름 : ColorTint
	const FName ColorOverlayParamName = FName("ColorTint");

	// 분노 버프 관련 변수
	FTimerHandle EnergizeTimerHandle;
	float OriginalWalkSpeed;

	float OriginalDefense;
};
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
	// --- Overrides ---
	virtual void BeginPlay() override;

	virtual void Die() override;

	virtual void Attack() override;

	// --- Skills ---
public:
	// Rampage의 광역 공격(Smash) 데미지 판정을 수행
	UFUNCTION(BlueprintCallable, Category = "Rampage Skills")
	void PerformSmashAttack();

	// Rampage의 분노(Energize) 버프 효과를 적용
	UFUNCTION(BlueprintCallable, Category = "Rampage Skills")
	void Energize();

protected:
	// 분노 버프 효과를 종료
	void EndEnergize();

	// --- 내부 멤버 변수 --- 
private:
	FTimerHandle EnergizeTimerHandle; // 분노 버프 지속시간을 제어하는 타이머 핸들

	float OriginalWalkSpeed; // 분노 버프 적용 전 원래 이동 속도 변수

	float OriginalDefense; // 분노 버프 적용 전 원래 방어력 변수

	// 분노 버프 시 색상 변경을 위한 머티리얼 인스턴스를 저장할 포인터 배열
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterialInstances; 

	const FName ColorOverlayParamName = FName("ColorTint"); // 머티리얼 색상 변경을 위해 사용하는 파라미터 이름





};
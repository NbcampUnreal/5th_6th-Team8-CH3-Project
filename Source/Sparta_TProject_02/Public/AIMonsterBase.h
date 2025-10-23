// AIMonsterBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIMonsterBase.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API AAIMonsterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AAIMonsterBase();

protected:
    // 게임 시작 시 호출
    virtual void BeginPlay() override;

public:
    // 매 프레임 호출
    virtual void Tick(float DeltaTime) override;

    // --- 몬스터 공통 스탯 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")
    float AttackDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")
    float AttackRange;

    // --- 몬스터 공통 기능 ---

    // 데미지를 받는 함수 (언리얼 기본 함수 오버라이드)
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // 공격 함수 (자식 클래스에서 구체적인 내용을 구현하도록 virtual로 선언)
    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    virtual void Attack();

    // 죽음 처리 함수
    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void Die();

protected:
    // 죽었는지 확인하는 플래그
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI State")
    bool bIsDead;
};
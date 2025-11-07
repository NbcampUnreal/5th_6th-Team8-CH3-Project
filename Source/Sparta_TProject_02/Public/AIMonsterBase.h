#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TimerHandle.h"
#include "AIMonsterBase.generated.h"

class UWidgetComponent;
class UMonsterHealthBar;
class USphereComponent;

UCLASS()
class SPARTA_TPROJECT_02_API AAIMonsterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AAIMonsterBase();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // --- AI Combat Stats (전투 스탯) ---
    UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "AI Stats")
    float BaseHealth; // 스테이지 변경을 위한 기초 체력(현재 미사용)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")
    float AttackDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")
    float Defense;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Stats")
    float AttackRange;

    // 죽었는지 확인하는 플래그
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI State")
    bool bIsDead;

    // 피격시 재생할 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Combat")
    class UAnimMontage* HitReactMontage;

    // 죽을 때 재생할 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Combat")
    class UAnimMontage* DeathMontage;

    // --- AI Combat & Damage (전투 기능) ---
    // 데미지를 받는 함수 (언리얼 기본 함수)
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    // 공격 함수
    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    virtual void Attack();

    // 죽음 처리 함수
    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    virtual void Die();

    // 스테이지에 따라 변경할 체력 배율
    void ApplyHealthMultiplier(float Multiplier);

protected:
    // 체력바를 표시할 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

    // 일정 시간 후 체력바를 숨기기 위한 타이머
    FTimerHandle HealthBarTimerHandle;

    // 체력바를 숨기는 함수
    void HideHealthBar();
private:
    // C++에서 위젯을 직접 제어하기 위한 포인터
    UPROPERTY()
    TObjectPtr<UMonsterHealthBar> HealthBarWidget;

protected:
    // 플레이어의 근접을 감지하기 위한 구체 충돌체
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<USphereComponent> AggroSphere;

    // 충돌체에 다른 액터가 들어왔을 때 호출될 이벤트 함수
    UFUNCTION()
    void OnAggroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
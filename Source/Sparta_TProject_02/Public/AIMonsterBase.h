// AIMonsterBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TimerHandle.h"
#include "AIMonsterBase.generated.h"

class ASTGameMode;
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
    // 게임 시작 시 호출
    virtual void BeginPlay() override;

public:
    // 매 프레임 호출
    virtual void Tick(float DeltaTime) override;

    // --- 몬스터 공통 스탯 ---
    //기본 체력 추가
    UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "AI Stats")
    float BaseHealth;

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
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    // 공격 함수 (자식 클래스에서 구체적인 내용을 구현하도록 virtual로 선언)
    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    virtual void Attack();

    // 죽음 처리 함수
    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    virtual void Die();

    bool IsDead() const { return bIsDead; }
    //체력 배율 반환
    void ApplyHealthMultiplier(float Multiplier);

protected:
    // 죽었는지 확인하는 플래그
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI State")
    bool bIsDead;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Combat")
    class UAnimMontage* HitReactMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Combat")
    class UAnimMontage* DeathMontage;

    // 체력바를 표시할 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

    // 일정 시간 후 체력바를 숨기기 위한 타이머
    FTimerHandle HealthBarTimerHandle;

    /** 체력바를 숨기는 함수 */
    void HideHealthBar();

    /** 플레이어 인지를 위한 구체 충돌체 (어그로 범위) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    TObjectPtr<USphereComponent> AggroSphere;

    /** AggroSphere에 다른 액터가 들어왔을 때 호출될 함수 */
    UFUNCTION()
    void OnAggroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    //GameMode에 알리기
    /*
    UPROPERTY()
    ASTGameMode* STGameMode;
    */

private:
    // C++에서 위젯을 직접 제어하기 위한 포인터
    UPROPERTY()
    TObjectPtr<UMonsterHealthBar> HealthBarWidget;
};
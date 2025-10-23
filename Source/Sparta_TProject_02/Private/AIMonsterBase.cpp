// AIMonsterBase.cpp

#include "AIMonsterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AAIMonsterBase::AAIMonsterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // 기본값 설정
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 10.0f;
    AttackRange = 150.0f;
    bIsDead = false;
}

void AAIMonsterBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth; // 게임 시작 시 체력 초기화
}

void AAIMonsterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float AAIMonsterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 이미 죽었다면 데미지 처리 안 함
    if (bIsDead) return 0.0f;

    // 부모 클래스의 TakeDamage 먼저 호출
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.0f)
    {
        CurrentHealth -= ActualDamage;
        UE_LOG(LogTemp, Warning, TEXT("%s's Health: %f"), *GetName(), CurrentHealth);

        if (CurrentHealth <= 0.0f)
        {
            Die();
        }
    }

    return ActualDamage;
}

// Attack 함수는 자식 클래스에서 구체적으로 구현할 것이므로 여기서는 비워두거나 기본 로그만 남깁니다.
void AAIMonsterBase::Attack()
{
    if (bIsDead) return;
    UE_LOG(LogTemp, Warning, TEXT("Base Attack Called on %s. This should be overridden!"), *GetName());
}

void AAIMonsterBase::Die()
{
    if (bIsDead) return;

    bIsDead = true;
    UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());

    // 충돌 비활성화
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // 움직임 정지
    GetCharacterMovement()->StopMovementImmediately();
    // AI 로직 정지 (AIController가 있다면)
    AController* AIController = GetController();
    if (AIController)
    {
        AIController->StopMovement();
    }

    // TODO: 죽는 애니메이션 재생 및 일정 시간 후 액터 파괴 로직 추가
    // 예: PlayAnimMontage(...)
    // SetLifeSpan(5.0f); // 5초 뒤에 사라짐
}
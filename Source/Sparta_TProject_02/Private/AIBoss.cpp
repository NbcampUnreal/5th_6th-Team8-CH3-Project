// AIBoss.cpp

#include "AIBoss.h"

AAIBoss::AAIBoss()
{
    // --- 보스 스탯 설정 ---
    MaxHealth = 1000.0f;     // 보스의 체력
    AttackDamage = 30.0f;   // 보스의 데미지
    AttackRange = 200.0f;   // 공격 범위(근접)
    Defense = 5.0f;         // 방어력
}

void AAIBoss::BeginPlay()
{
    Super::BeginPlay();

}

// 보스 공격 로직
void AAIBoss::Attack()
{
    // 부모의 Attack 함수 호출
    // UE_LOG(LogTemp, Warning, TEXT("Boss is attacking!"));
    Super::Attack();

    if (bIsDead) return;
}

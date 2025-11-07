// AIZombie.cpp

#include "AIZombie.h"

AAIZombie::AAIZombie()
{
    // --- 좀비 스탯 설정 ---
    MaxHealth = 100.0f;     // 좀비의 체력
    AttackDamage = 15.0f;   // 좀비의 데미지
    AttackRange = 100.0f;   // 공격 범위(근접)
    Defense = 0.0f;         // 방어력(없음)
}

// 좀비 공격 로직
void AAIZombie::Attack()
{
    // 부모의 Attack 함수 호출
    // UE_LOG(LogTemp, Warning, TEXT("Zombie is attacking!"));
    Super::Attack();

    if (bIsDead) return;
}
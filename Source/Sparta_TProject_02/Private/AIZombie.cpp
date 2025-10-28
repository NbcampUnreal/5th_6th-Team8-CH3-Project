// AIZombie.cpp

#include "AIZombie.h"

AAIZombie::AAIZombie()
{
    // 좀비에 맞는 기본 스탯으로 변경
    MaxHealth = 80.0f;
    AttackDamage = 15.0f;
    AttackRange = 100.0f; // 근접 공격 범위
}

void AAIZombie::Attack()
{
    // 부모의 Attack 함수를 먼저 호출 (선택사항)
    Super::Attack();

    if (bIsDead) return;

    UE_LOG(LogTemp, Warning, TEXT("Zombie is attacking!"));

    //if (AttackMontage)
    //{
    //    // 몽타주 재생
    //    PlayAnimMontage(AttackMontage);
    //    UE_LOG(LogTemp, Warning, TEXT("%s is playing AttackMontage!"), *GetName());
    //}
    //else
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("%s has no AttackMontage assigned!"), *GetName());
    //}

}
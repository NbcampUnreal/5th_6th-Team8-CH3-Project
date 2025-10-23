// AIBoss.cpp

#include "AIBoss.h"

AAIBoss::AAIBoss()
{
    // 보스에 맞는 강력한 스탯으로 변경
    MaxHealth = 1000.0f;
    AttackDamage = 50.0f;
    AttackRange = 250.0f;
}

void AAIBoss::Attack()
{
    Super::Attack();

    if (bIsDead) return;

    //UE_LOG(LogTemp, Warning, TEXT("Boss is performing a normal attack!"));

    //if (NormalAttackMontage)
    //{
    //    PlayAnimMontage(NormalAttackMontage);
    //}
}

//void AAIBoss::PerformSpecialAttack()
//{
//    if (bIsDead) return;
//
//    UE_LOG(LogTemp, Warning, TEXT("Boss is performing a SPECIAL attack!"));
//
//    if (SpecialAttackMontage)
//    {
//        PlayAnimMontage(SpecialAttackMontage);
//    }
//}
// AIBoss.cpp

#include "AIBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

AAIBoss::AAIBoss()
{
    // 보스에 맞는 강력한 스탯으로 변경
    MaxHealth = 1000.0f;
    AttackDamage = 50.0f;
    AttackRange = 250.0f;
}

void AAIBoss::BeginPlay()
{
    Super::BeginPlay();

}

void AAIBoss::Attack()
{
    Super::Attack();
    if (bIsDead) return;
}

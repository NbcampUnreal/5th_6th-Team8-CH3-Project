// AIMonsterBase.cpp

#include "AIMonsterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sparta_TProject_02/Sparta_TProject_02Character.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "UObject/UObjectGlobals.h"
#include "MonsterHealthBar.h"

AAIMonsterBase::AAIMonsterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // 기본값 설정
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 10.0f;
    AttackRange = 150.0f;
    bIsDead = false;
    bIsAttacking = false;

    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);

    HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidgetComponent->SetupAttachment(GetMesh());
    HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidgetComponent->SetDrawAtDesiredSize(true);
    HealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
}

void AAIMonsterBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth; // 게임 시작 시 체력 초기화

    if (HealthBarWidgetComponent)
    {
        HealthBarWidgetComponent->SetVisibility(false);
    }
}

void AAIMonsterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAIMonsterBase::HideHealthBar()
{
    if (HealthBarWidgetComponent)
    {
        HealthBarWidgetComponent->SetVisibility(false);
    }
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
        
        if (HealthBarWidgetComponent)
        {
            HealthBarWidgetComponent->SetVisibility(true);

            UMonsterHealthBar* HealthBar = Cast<UMonsterHealthBar>(HealthBarWidgetComponent->GetUserWidgetObject());

            if (HealthBar)
            {
                float HealthRatio = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
                HealthBar->UpdateHealthBar(HealthRatio);
            }
            GetWorldTimerManager().SetTimer(HealthBarTimerHandle, this, &AAIMonsterBase::HideHealthBar, 3.0f, false);
        }

        UE_LOG(LogTemp, Warning, TEXT("%s's Health: %f"), *GetName(), CurrentHealth);

        if (CurrentHealth <= 0.0f)
        {
            Die();
        }
        else
        {
            if (HitReactMontage)
            {
                PlayAnimMontage(HitReactMontage);
            }
        }
    }

    return ActualDamage;
}

void AAIMonsterBase::Attack()
{
    if (bIsDead) return;

    FVector StartPoint = GetActorLocation();
    FVector EndPoint = StartPoint + (GetActorForwardVector() * AttackRange);

    FCollisionShape Sphere = FCollisionShape::MakeSphere(50.0f);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    TArray<FHitResult> HitResults;

    bool bIsHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        StartPoint,
        EndPoint,
        FQuat::Identity,
        ECC_Pawn,
        Sphere,
        Params
    );

    if (bIsHit)
    {
        for (const auto& Hit : HitResults)
        {
            ASparta_TProject_02Character* PlayerCharacter = Cast<ASparta_TProject_02Character>(Hit.GetActor());
            if (PlayerCharacter)
            {
                UE_LOG(LogTemp, Warning, TEXT("%s's attack hit %s! Applying %f damage."), *GetName(), *PlayerCharacter->GetName(), AttackDamage);
                
                UGameplayStatics::ApplyDamage(
                    PlayerCharacter,
                    AttackDamage,
                    GetController(),
                    this,
                    nullptr
                );
                break;
            }
        }
    }
}

void AAIMonsterBase::Die()
{
    if (bIsDead) return;

    bIsDead = true;
    UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());

    if (HealthBarWidgetComponent)
    {
        HealthBarWidgetComponent->SetVisibility(false);
    }

    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController && AIController->GetBrainComponent())
    {
        AIController->GetBrainComponent()->StopLogic("Died");
    }

    // 충돌 비활성화
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    USkeletalMeshComponent* MyMesh = GetMesh();
    if (MyMesh)
    {
        MyMesh->SetSimulatePhysics(true);
        MyMesh->SetCollisionProfileName(TEXT("Ragdoll"));
    }

    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage, 1.0f);
    }

    SetLifeSpan(5.0f);
}
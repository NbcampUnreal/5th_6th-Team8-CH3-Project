#include "GrenadeActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

AGrenadeActor::AGrenadeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
    MeshComp->SetCollisionProfileName("BlockAllDynamic");
    MeshComp->SetSimulatePhysics(false); // ProjectileMovement가 움직임 담당

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 0.3f;
    ProjectileMovement->ProjectileGravityScale = 1.0f;
    ProjectileMovement->InitialSpeed = 1000.f;
    ProjectileMovement->MaxSpeed = 1500.f;
}

void AGrenadeActor::BeginPlay()
{
    Super::BeginPlay();

    // 3초 후 폭발
    GetWorldTimerManager().SetTimer(FuseTimerHandle, this, &AGrenadeActor::Explode, FuseTime, false);
}

void AGrenadeActor::Explode()
{
    // 폭발 이펙트
    if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
    }

    // 폭발 사운드
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
    }

    // 데미지 처리
    UGameplayStatics::ApplyRadialDamage(
        this,
        ExplosionDamage,
        GetActorLocation(),
        ExplosionRadius,
        nullptr,
        TArray<AActor*>(),
        this,
        GetInstigatorController(),
        true
    );

    Destroy();
}

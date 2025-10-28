#include "GunBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

AGunBase::AGunBase()
{
    PrimaryActorTick.bCanEverTick = false;

    GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
    RootComponent = GunMesh;

    Damage = 20.0f;
    MaxMagazineAmmo = 30;
    CurrentAmmo = MaxMagazineAmmo;
    FireRate = 600.0f;
    WeaponType = EWeaponType::WT_Pistol;

    bIsReloading = false;
    OwningPlayer = nullptr;
}

void AGunBase::BeginPlay()
{
    Super::BeginPlay();

    OwningPlayer = Cast<APlayerCharacter>(GetOwner());
}

void AGunBase::SetWeaponHidden(bool bShouldBeHidden)
{
    SetActorHiddenInGame(bShouldBeHidden);
    SetActorEnableCollision(!bShouldBeHidden);
    SetActorTickEnabled(!bShouldBeHidden);
}

void AGunBase::StartFire()
{
    if (bIsReloading) return;

    // if already firing via timer, ignore
    if (GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle)) return;

    OnStartFire.Broadcast();

    if (FireRate <= 0.f)
    {
        TraceFire();
    }
    else
    {
        float TimeBetweenShots = 60.0f / FireRate;
        TraceFire();
        GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AGunBase::TraceFire, TimeBetweenShots, true);
    }
}

void AGunBase::StopFire()
{
    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
    OnStopFire.Broadcast();
}

void AGunBase::Reload()
{
    if (bIsReloading) return;
    if (!OwningPlayer) return;

    // if weapon uses reserve ammo, check
    if (WeaponType != EWeaponType::WT_Pistol && OwningPlayer->GetReserveAmmo(WeaponType) <= 0)
    {
        return;
    }

    // 탄창이 가득 차 있으면 리로드 불가능
    if (CurrentAmmo == MaxMagazineAmmo)
    {
        return;
    }

    bIsReloading = true;
    StopFire();
    OnStartReload.Broadcast();

    float ReloadTime = 3.0f;
    // << [수정 4] OwningPlayer->GetFPMesh() 호출을 통해 FP_Mesh에 접근합니다.
    if (ReloadMontage && OwningPlayer && OwningPlayer->GetFPMesh() && OwningPlayer->GetFPMesh()->GetAnimInstance())
    {
        ReloadTime = ReloadMontage->GetPlayLength();
    }

    GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AGunBase::FinishReload, ReloadTime, false);
}

void AGunBase::FinishReload()
{
    if (!OwningPlayer)
    {
        bIsReloading = false;
        OnFinishReload.Broadcast();
        return;
    }

    int32 AmmoToReload = MaxMagazineAmmo - CurrentAmmo;
    if (AmmoToReload <= 0)
    {
        bIsReloading = false;
        OnFinishReload.Broadcast();
        return;
    }

    int32 AmmoConsumed = OwningPlayer->ConsumeAmmoForReload(WeaponType, AmmoToReload);
    CurrentAmmo += AmmoConsumed;

    bIsReloading = false;
    GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
    OnFinishReload.Broadcast();
}

void AGunBase::TraceFire()
{
    if (CurrentAmmo <= 0 || bIsReloading)
    {
        StopFire();
        return;
    }

    CurrentAmmo--;

    if (!OwningPlayer)
    {
        OwningPlayer = Cast<APlayerCharacter>(GetOwner());
    }
    if (!OwningPlayer) return;

    AController* OwnerController = OwningPlayer->GetController();
    if (!OwnerController) return;

    // Effects
    if (MuzzleFlash && GunMesh)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GunMesh, FName("Muzzle"));
    }
    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }

    if (FireMontage && OwningPlayer->GetFPMesh() && OwningPlayer->GetFPMesh()->GetAnimInstance())
    {
        // Optionally play via character delegate; but keep here for safety if not bound
        //OwningPlayer->GetFPMesh()->GetAnimInstance()->Montage_Play(FireMontage);
    }

    FVector StartLocation;
    FRotator CameraRotation;
    OwnerController->GetPlayerViewPoint(StartLocation, CameraRotation);
    FVector TraceEnd = StartLocation + (CameraRotation.Vector() * 10000.f);

    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, TraceEnd, ECC_Visibility, QueryParams))
    {
        UGameplayStatics::ApplyPointDamage(
            Hit.GetActor(),
            Damage,
            CameraRotation.Vector(),
            Hit,
            OwnerController,
            this,
            nullptr
        );
    }
}
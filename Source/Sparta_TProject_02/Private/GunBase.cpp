#include "GunBase.h"
#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h" 
#include "GameFramework/Pawn.h" 
#include "Animation/AnimInstance.h" 
#include "Particles/ParticleSystem.h" 

AGunBase::AGunBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;

	WeaponType = EWeaponType::WT_Pistol;
	Damage = 20.0f;
	MaxMagazineAmmo = 30;
	FireRate = 600.0f;

	CurrentAmmo = MaxMagazineAmmo;
	bIsReloading = false;

	OwningPlayer = nullptr;
}

void AGunBase::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayer = Cast<APlayerCharacter>(GetOwner()); // ������ ĳ���� �� ����
}

void AGunBase::TraceFire()
{
	if (CurrentAmmo <= 0 || bIsReloading)
	{
		StopFire();
		return;
	}

	CurrentAmmo--;

	if (!OwningPlayer) return;

	AController* OwnerController = OwningPlayer->GetController();
	if (!OwnerController) return;

	// --- �߰�: ����Ʈ ��� ---
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GunMesh, FName("Muzzle"));
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	// 1��Ī �� �޽����� �߻� ��Ÿ�� ���
	if (FireMontage && OwningPlayer->GetFPMesh() && OwningPlayer->GetFPMesh()->GetAnimInstance())
	{
		OwningPlayer->GetFPMesh()->GetAnimInstance()->Montage_Play(FireMontage);
	}
	// --- ����Ʈ ��� �� ---


	FVector StartLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(StartLocation, CameraRotation);

	FVector TraceEnd = StartLocation + CameraRotation.Vector() * 10000.f;

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

void AGunBase::Shoot()
{
	StartFire();
}

void AGunBase::StartFire()
{
	if (bIsReloading || GetWorldTimerManager().IsTimerActive(FireTimerHandle)) return;

	if (FireRate <= 0.0f)
	{
		TraceFire();
	}
	else
	{
		float TimeBetweenShots = 60.0f / FireRate;
		TraceFire();
		GetWorldTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&AGunBase::TraceFire,
			TimeBetweenShots,
			true
		);
	}
}

void AGunBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AGunBase::Reload()
{
	//����: ������ ���� (���� źâ �˻� �߰�)
	if (bIsReloading || CurrentAmmo == MaxMagazineAmmo || !OwningPlayer) return;

	if (WeaponType != EWeaponType::WT_Pistol)
	{
		if (OwningPlayer->GetReserveAmmo(WeaponType) <= 0)
		{
			return;
		}
	}

	bIsReloading = true;
	StopFire();

	float ReloadTime = 3.0f;
	if (ReloadMontage && OwningPlayer->GetFPMesh() && OwningPlayer->GetFPMesh()->GetAnimInstance())
	{
		OwningPlayer->GetFPMesh()->GetAnimInstance()->Montage_Play(ReloadMontage);
		ReloadTime = ReloadMontage->GetPlayLength();
	}

	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&AGunBase::FinishReload,
		ReloadTime,
		false
	);
}

void AGunBase::FinishReload()
{
	// ����: ������ �Ϸ� ���� (���� ź�� �Ҹ� ����)
	bIsReloading = false;
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

	if (!OwningPlayer) return;

	int32 AmmoToReload = MaxMagazineAmmo - CurrentAmmo;
	if (AmmoToReload <= 0) return;

	// APlayerCharacter�� public �Լ��� ȣ���Ͽ� ź�� �Ҹ�
	int32 AmmoConsumed = OwningPlayer->ConsumeAmmoForReload(WeaponType, AmmoToReload);

	CurrentAmmo += AmmoConsumed;
}

// ����: �Ű����� �̸� bHidden -> bShouldBeHidden���� ����
void AGunBase::SetWeaponHidden(bool bShouldBeHidden)
{
	SetActorHiddenInGame(bShouldBeHidden);
	SetActorEnableCollision(!bShouldBeHidden);
	SetActorTickEnabled(!bShouldBeHidden);
}
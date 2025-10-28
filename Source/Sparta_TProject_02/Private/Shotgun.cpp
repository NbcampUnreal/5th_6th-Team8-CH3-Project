#include "Shotgun.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"

AShotgun::AShotgun()
{
	Damage = 15.0f;
	MaxMagazineAmmo = 8;
	CurrentAmmo = MaxMagazineAmmo;
	FireRate = 0.0f;
	PelletsPerShot = 8;
	SpreadAngle = 5.0f;
	WeaponType = EWeaponType::WT_Shotgun;
}

void AShotgun::TraceFire()
{
	if (CurrentAmmo <= 0 || bIsReloading)
	{
		StopFire();
		return;
	}

	CurrentAmmo--;

	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player) return;

	AController* OwnerController = Player->GetController();
	if (!OwnerController) return;

	if (MuzzleFlash && GunMesh)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GunMesh, FName("Muzzle"));
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	FVector StartLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(StartLocation, CameraRotation);

	FVector ShootDir = CameraRotation.Vector();
	float ConeHalfAngleRad = FMath::DegreesToRadians(SpreadAngle * 0.5f);

	for (int32 i = 0; i < PelletsPerShot; ++i)
	{
		FVector SpreadVector = UKismetMathLibrary::RandomUnitVectorInConeInRadians(ShootDir, ConeHalfAngleRad);
		FVector TraceEnd = StartLocation + (SpreadVector * 10000.f);

		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(GetOwner());

		if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			UGameplayStatics::ApplyPointDamage(
				Hit.GetActor(),
				Damage,
				SpreadVector,
				Hit,
				OwnerController,
				this,
				nullptr
			);
		}
	}
}
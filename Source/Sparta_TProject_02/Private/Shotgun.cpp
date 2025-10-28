#include "Shotgun.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h" 
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "PlayerCharacter.h" // �߰�: OwningPlayer ����� ����

AShotgun::AShotgun()
{
	Damage = 15.0f;
	MaxMagazineAmmo = 8;
	FireRate = 0.0f;

	PelletsPerShot = 8;
	SpreadAngle = 5.0f;

	WeaponType = EWeaponType::WT_Shotgun; // �߰�: ���� Ÿ�� ���
}

void AShotgun::TraceFire()
{
	if (CurrentAmmo <= 0 || bIsReloading)
	{
		StopFire();
		return;
	}

	CurrentAmmo--;

	// ����: OwnerPawn ��� ĳ���ص� OwningPlayer ���
	if (!OwningPlayer) return;
	AController* OwnerController = OwningPlayer->GetController();
	if (!OwnerController) return;

	// --- �߰�: ���� ����Ʈ ��� (TraceFire�� �� ���� ȣ��ǹǷ� ���⿡ �־� ��) ---
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GunMesh, FName("Muzzle"));
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	if (FireMontage && OwningPlayer->GetFPMesh() && OwningPlayer->GetFPMesh()->GetAnimInstance())
	{
		OwningPlayer->GetFPMesh()->GetAnimInstance()->Montage_Play(FireMontage);
	}
	// --- ����Ʈ ��� �� ---


	FVector StartLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(StartLocation, CameraRotation);

	FVector ShootDir = CameraRotation.Vector();
	float ConeHalfAngleRad = FMath::DegreesToRadians(SpreadAngle * 0.5f);

	for (int32 i = 0; i < PelletsPerShot; i++)
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
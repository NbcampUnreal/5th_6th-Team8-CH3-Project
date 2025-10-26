#include "Shotgun.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h" 
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

AShotgun::AShotgun()
{
	Damage = 15.0f; // �縴 �� �����
	MaxMagazineAmmo = 8;
	FireRate = 0.0f;  // ���ڵ����� ���� (AGunBase�� �˾Ƽ� ó��)

	PelletsPerShot = 8; // �縴 8��
	SpreadAngle = 5.0f;
}

// ������ �ٽ� ����: ���� �縴 Ʈ���̽�
void AShotgun::TraceFire()
{
	if (CurrentAmmo <= 0 || bIsReloading)
	{
		StopFire();
		return;
	}

	CurrentAmmo--;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	AController* OwnerController = OwnerPawn->GetController();
	if (!OwnerController) return;

	FVector StartLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(StartLocation, CameraRotation);

	FVector ShootDir = CameraRotation.Vector();
	float ConeHalfAngleRad = FMath::DegreesToRadians(SpreadAngle * 0.5f);

	for (int32 i = 0; i < PelletsPerShot; i++)
	{
		// KismetMathLibrary�� ����� SpreadAngle ������ ������ ���� ���� ����
		FVector SpreadVector = UKismetMathLibrary::RandomUnitVectorInConeInRadians(ShootDir, ConeHalfAngleRad);
		FVector TraceEnd = StartLocation + (SpreadVector * 10000.f); // ��Ÿ� 10000

		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(GetOwner());

		if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// �縴 �� ����� ����
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
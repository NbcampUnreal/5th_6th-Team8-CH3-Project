#include "Shotgun.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h" 
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

AShotgun::AShotgun()
{
	Damage = 15.0f; // 펠릿 당 대미지
	MaxMagazineAmmo = 8;
	FireRate = 0.0f;  // 반자동으로 설정 (AGunBase가 알아서 처리)

	PelletsPerShot = 8; // 펠릿 8개
	SpreadAngle = 5.0f;
}

// 샷건의 핵심 로직: 다중 펠릿 트레이스
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
		// KismetMathLibrary를 사용해 SpreadAngle 내에서 랜덤한 방향 벡터 생성
		FVector SpreadVector = UKismetMathLibrary::RandomUnitVectorInConeInRadians(ShootDir, ConeHalfAngleRad);
		FVector TraceEnd = StartLocation + (SpreadVector * 10000.f); // 사거리 10000

		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(GetOwner());

		if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// 펠릿 당 대미지 적용
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
#include "Turret.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Projectile.h"
#include "AIMonsterBase.h"
#include "TimerManager.h"
#include "TurretAnimInstance.h"


ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh
	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretMesh"));
	RootComponent = TurretMesh;

	// Detection
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetSphereRadius(1000.f);

	// ±âº»°ª
	bHasTarget = false;

	Health = 100.f;
	Damage = 20.f;
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnSound, GetActorLocation());
	}

	CurrentTurretRotation = GetActorRotation();

	GetWorldTimerManager().SetTimer(ScanTimerHandle, this, &ATurret::ScanForTargets, 0.3f, true);
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATurret::Fire, FireInterval, true);
	GetWorldTimerManager().PauseTimer(FireTimerHandle);

	GetWorldTimerManager().SetTimer(LifeTimerHandle, this, &ATurret::DestroySelf, LifeTime, false);
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasTarget && CurrentTarget)
	{
		AimAtTarget(DeltaTime);
		if (GetWorldTimerManager().IsTimerPaused(FireTimerHandle))
			GetWorldTimerManager().UnPauseTimer(FireTimerHandle);
	}
	else
	{
		RotateIdle(DeltaTime);
		if (!GetWorldTimerManager().IsTimerPaused(FireTimerHandle))
			GetWorldTimerManager().PauseTimer(FireTimerHandle);
	}

	if (UTurretAnimInstance* AnimInst = Cast<UTurretAnimInstance>(TurretMesh->GetAnimInstance()))
	{
		AnimInst->BarrelOffset = FMath::FInterpTo(AnimInst->BarrelOffset, 0.f, DeltaTime, 15.f);
	}
}

void ATurret::RotateIdle(float DeltaTime)
{
	static float Direction = 1.f;

	if (UTurretAnimInstance* AnimInst = Cast<UTurretAnimInstance>(TurretMesh->GetAnimInstance()))
	{
		float CurrentYaw = AnimInst->HeadRotation.Yaw;
		float NewYaw = CurrentYaw + Direction * RotationSpeed * DeltaTime;

		if (FMath::Abs(NewYaw) >= 60.f)
		{
			Direction *= -1.f;
			NewYaw = FMath::Clamp(NewYaw, -60.f, 60.f);
		}

		AnimInst->HeadRotation.Yaw = NewYaw;
		AnimInst->HeadRotation.Pitch = 0.f;
	}
}

void ATurret::ScanForTargets()
{
	TArray<AActor*> OverlappingActors;
	DetectionSphere->GetOverlappingActors(OverlappingActors, AAIMonsterBase::StaticClass());

	AActor* ClosestTarget = nullptr;
	float MinDistSq = TNumericLimits<float>::Max();
	FVector MyLocation = GetActorLocation();

	for (AActor* Target : OverlappingActors)
	{
		if (!Target) continue;
		float DistSq = FVector::DistSquared(MyLocation, Target->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			ClosestTarget = Target;
		}
	}

	bHasTarget = (ClosestTarget != nullptr);
	CurrentTarget = ClosestTarget;
}

void ATurret::AimAtTarget(float DeltaTime)
{
	if (!CurrentTarget) return;

	FVector Dir = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator TargetWorldRot = Dir.Rotation();

	FRotator TurretWorldRot = GetActorRotation();

	CurrentTurretRotation = FMath::RInterpTo(CurrentTurretRotation, TargetWorldRot, DeltaTime, 5.f);

	FRotator RelativeRot = UKismetMathLibrary::NormalizedDeltaRotator(CurrentTurretRotation, TurretWorldRot); 

	if (UTurretAnimInstance* AnimInst = Cast<UTurretAnimInstance>(TurretMesh->GetAnimInstance()))
	{
		AnimInst->HeadRotation.Yaw = RelativeRot.Yaw;
		AnimInst->HeadRotation.Pitch = RelativeRot.Pitch;
		AnimInst->HeadRotation.Roll = 0.f;
	}
}

void ATurret::Fire()
{
	if (!bHasTarget || !CurrentTarget || !ProjectileClass) return;

	FVector MuzzleLoc = TurretMesh->GetSocketLocation(FName("Muzzle"));
	FRotator MuzzleRot = TurretMesh->GetSocketRotation(FName("Muzzle"));

	if (MuzzleFX)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, MuzzleLoc, MuzzleRot);

	if (FireSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, MuzzleLoc);

	if (UTurretAnimInstance* AnimInst = Cast<UTurretAnimInstance>(TurretMesh->GetAnimInstance()))
	{
		AnimInst->BarrelOffset = -5.f; 
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = Cast<APawn>(this);
	GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLoc, MuzzleRot, Params);


}



void ATurret::TakeTurretDamage(float Amount)
{
	Health -= Amount;
	if (Health <= 0.f)
	{
		DestroySelf();
	}
}

void ATurret::DestroySelf()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	if (DestroyEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorLocation(), GetActorRotation());
	}
	if (DestroySound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, GetActorLocation());
	}

	Destroy();
}
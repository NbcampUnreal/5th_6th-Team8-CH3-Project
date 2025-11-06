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

	// 기본값
	bHasTarget = false;

	Health = 100.f;
	Damage = 20.f;
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();

	// 터렛의 현재 월드 회전으로 초기화. 조준 시 이 값을 기준으로 보간합니다.
	CurrentTurretRotation = GetActorRotation();

	GetWorldTimerManager().SetTimer(ScanTimerHandle, this, &ATurret::ScanForTargets, 0.3f, true);
	// 타겟이 없으면 멈춘 상태로 시작
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
		// 타겟이 있을 때만 발사 타이머가 돌아가도록 관리
		if (GetWorldTimerManager().IsTimerPaused(FireTimerHandle))
			GetWorldTimerManager().UnPauseTimer(FireTimerHandle);
	}
	else
	{
		RotateIdle(DeltaTime);
		// 타겟을 잃으면 발사 타이머 정지
		if (!GetWorldTimerManager().IsTimerPaused(FireTimerHandle))
			GetWorldTimerManager().PauseTimer(FireTimerHandle);
	}

	if (UTurretAnimInstance* AnimInst = Cast<UTurretAnimInstance>(TurretMesh->GetAnimInstance()))
	{
		// 서서히 BarrelOffset 복귀 (리코일 되돌리기)
		AnimInst->BarrelOffset = FMath::FInterpTo(AnimInst->BarrelOffset, 0.f, DeltaTime, 15.f);
	}
}

void ATurret::RotateIdle(float DeltaTime)
{
	static float Direction = 1.f;

	if (UTurretAnimInstance* AnimInst = Cast<UTurretAnimInstance>(TurretMesh->GetAnimInstance()))
	{
		// 애님 인스턴스의 HeadRotation을 기준으로 회전 계산
		float CurrentYaw = AnimInst->HeadRotation.Yaw;
		float NewYaw = CurrentYaw + Direction * RotationSpeed * DeltaTime;

		// -60도 ~ 60도 제한 (Head Bone의 로컬 회전 기준)
		if (FMath::Abs(NewYaw) >= 60.f)
		{
			Direction *= -1.f;
			NewYaw = FMath::Clamp(NewYaw, -60.f, 60.f);
		}

		// Yaw만 회전 적용
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

	// 상태 업데이트
	bHasTarget = (ClosestTarget != nullptr);
	CurrentTarget = ClosestTarget;
}

void ATurret::AimAtTarget(float DeltaTime)
{
	if (!CurrentTarget) return;

	FVector Dir = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator TargetWorldRot = Dir.Rotation();

	FRotator TurretWorldRot = GetActorRotation();

	// 부드러운 회전을 위해 CurrentTurretRotation(월드 회전)을 TargetWorldRot으로 보간
	CurrentTurretRotation = FMath::RInterpTo(CurrentTurretRotation, TargetWorldRot, DeltaTime, 5.f);

	FRotator RelativeRot = UKismetMathLibrary::NormalizedDeltaRotator(CurrentTurretRotation, TurretWorldRot); 

	if (UTurretAnimInstance* AnimInst = Cast<UTurretAnimInstance>(TurretMesh->GetAnimInstance()))
	{
		// Yaw와 Pitch만 사용
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

	// 리코일 효과: 애님 인스턴스에 전달
	if (UTurretAnimInstance* AnimInst = Cast<UTurretAnimInstance>(TurretMesh->GetAnimInstance()))
	{
		AnimInst->BarrelOffset = -5.f; // 포신 뒤로 살짝 당김
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
		Destroy();
	}
}

void ATurret::DestroySelf()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	Destroy();
}
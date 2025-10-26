#include "GunBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AGunBase::AGunBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;

	Damage = 20.0f;
	MaxMagazineAmmo = 30;
	FireRate = 600.0f; 

	CurrentAmmo = MaxMagazineAmmo;
	bIsReloading = false;
}

void AGunBase::BeginPlay()
{
	Super::BeginPlay();

}

void AGunBase::TraceFire()
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

	// 1. 발사 속도(FireRate)가 0인지 아닌지 먼저 검사
	if (FireRate <= 0.0f)
	{
		// 발사 속도가 0이하면 (반자동), TraceFire 한 발만 쏘고 끝
		TraceFire();
	}
	else
	{
		// 발사 속도가 0보다 크면 (자동)
		float TimeBetweenShots = 60.0f / FireRate;

		// 일단 한 발 쏘고
		TraceFire();

		// 타이머 설정
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
	if (bIsReloading || CurrentAmmo == MaxMagazineAmmo) return;

	bIsReloading = true;

	// 재장전 시 발사 중지 (필수!)
	StopFire();

	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle, // <-- FireTimerHandle 대신 ReloadTimerHandle 사용
		this,
		&AGunBase::FinishReload,
		3.0f,
		false
	);
}

void AGunBase::FinishReload()
{
	CurrentAmmo = MaxMagazineAmmo;
	bIsReloading = false;

	// FireTimerHandle을 건드리지 않고, ReloadTimerHandle만 제거
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
}
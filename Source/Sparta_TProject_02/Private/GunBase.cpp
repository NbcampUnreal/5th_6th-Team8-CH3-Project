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

	// 1. �߻� �ӵ�(FireRate)�� 0���� �ƴ��� ���� �˻�
	if (FireRate <= 0.0f)
	{
		// �߻� �ӵ��� 0���ϸ� (���ڵ�), TraceFire �� �߸� ��� ��
		TraceFire();
	}
	else
	{
		// �߻� �ӵ��� 0���� ũ�� (�ڵ�)
		float TimeBetweenShots = 60.0f / FireRate;

		// �ϴ� �� �� ���
		TraceFire();

		// Ÿ�̸� ����
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

	// ������ �� �߻� ���� (�ʼ�!)
	StopFire();

	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle, // <-- FireTimerHandle ��� ReloadTimerHandle ���
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

	// FireTimerHandle�� �ǵ帮�� �ʰ�, ReloadTimerHandle�� ����
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
}
#include "HealthPackSpawner.h"
#include "HealthPack.h"

AHealthPackSpawner::AHealthPackSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	StaticMeshCompoent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshCompoent->SetupAttachment(RootComponent);

	HealthPackClass = nullptr;
	SpawnedHealthPack = nullptr;

	SpawnTime = 30.0f;
}

void AHealthPackSpawner::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AHealthPackSpawner::SpawnHealthPack, SpawnTime, true);
}

void AHealthPackSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AHealthPackSpawner::SpawnHealthPack()
{
	if (IsValid(SpawnedHealthPack)) return;

	FVector Pos = GetActorLocation();
	Pos.Z += 20.0f;
	if (HealthPackClass)
	{
		SpawnedHealthPack = GetWorld()->SpawnActor<AHealthPack>(HealthPackClass, Pos, GetActorRotation());
	}
	else
	{
		SpawnedHealthPack = GetWorld()->SpawnActor<AHealthPack>(Pos, GetActorRotation());
	}
}

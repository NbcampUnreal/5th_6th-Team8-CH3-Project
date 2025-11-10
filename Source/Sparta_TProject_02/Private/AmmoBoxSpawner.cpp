#include "AmmoBoxSpawner.h"
#include "AmmoBox.h"

AAmmoBoxSpawner::AAmmoBoxSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	StaticMeshCompoent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshCompoent->SetupAttachment(RootComponent);

	AmmoBoxClass = nullptr;
	SpawnedAmmoBox = nullptr;

	SpawnTime = 30.0f;
}

void AAmmoBoxSpawner::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AAmmoBoxSpawner::SpawnAmmoBox, SpawnTime, true);
}

void AAmmoBoxSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AAmmoBoxSpawner::SpawnAmmoBox()
{
	if (IsValid(SpawnedAmmoBox)) return;

	FVector Pos = GetActorLocation();
	Pos.Z += 20.0f;
	if (AmmoBoxClass)
	{
		SpawnedAmmoBox = GetWorld()->SpawnActor<AAmmoBox>(AmmoBoxClass, Pos, GetActorRotation());
	}
	else
	{
		SpawnedAmmoBox = GetWorld()->SpawnActor<AAmmoBox>(Pos, GetActorRotation());
	}
}
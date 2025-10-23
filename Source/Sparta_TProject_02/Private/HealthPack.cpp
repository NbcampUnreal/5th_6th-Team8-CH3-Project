#include "HealthPack.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
AHealthPack::AHealthPack()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	StaticMeshCompoent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshCompoent->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereComponent->SetupAttachment(RootComponent);
}

void AHealthPack::BeginPlay()
{
	Super::BeginPlay();
	BobbingStartpos = GetActorLocation().Z;
	BobbingHeight = 100.0f;
	//GetWorldTimerManager().SetTimer(BobbingTimerHandle, this, &AHealthPack::Bobbing, 0.1f, true);
}

void AHealthPack::Bobbing() {
	float CurrentTime = GetWorld()->GetRealTimeSeconds();
	FVector NewActorLocation = GetActorLocation();
	NewActorLocation.Z = BobbingStartpos + (BobbingHeight * FMath::Sin(CurrentTime));
	SetActorLocation(NewActorLocation);
}
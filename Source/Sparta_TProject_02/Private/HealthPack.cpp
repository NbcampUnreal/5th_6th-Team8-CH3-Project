#include "HealthPack.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.h"

AHealthPack::AHealthPack()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	StaticMeshCompoent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshCompoent->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereComponent->SetupAttachment(RootComponent);

	SphereComponent->SetCollisionProfileName("OverlapAllDynamic");
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AHealthPack::OnOverlap);

	bUsePercentHeal = true;
	HealValue = 40.0f;
}

void AHealthPack::BeginPlay()
{
	Super::BeginPlay();
	BobbingStartpos = GetActorLocation().Z;
	BobbingHeight = 20.0f;
	GetWorldTimerManager().SetTimer(BobbingTimerHandle, this, &AHealthPack::Bobbing, 0.03f, true);
}

void AHealthPack::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(BobbingTimerHandle);
	Super::EndPlay(EndPlayReason);
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Healkit"));
}

void AHealthPack::Bobbing()
{
	float CurrentTime = GetWorld()->GetRealTimeSeconds();
	FVector NewActorLocation = GetActorLocation();
	float BobbingOffset = (FMath::Sin(CurrentTime) + 1.0f) * 0.5f;
	NewActorLocation.Z = BobbingStartpos + (BobbingHeight * BobbingOffset);
	SetActorLocation(NewActorLocation);
}

void AHealthPack::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	APawn* PawnActor = Cast<APawn>(OtherActor);
	if (!PawnActor) return;

	ACharacter* Character = Cast<ACharacter>(PawnActor);
	if (!Character) return;

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Character);
	if (!PlayerCharacter) return;

	float Health = PlayerCharacter->GetHealth();
	float MaxHealth = PlayerCharacter->GetMaxHealth();
	float Heal = 50.0f;

	if (bUsePercentHeal)	// 퍼센트 회복
	{
		Health += (HealValue / 100.0f) * MaxHealth;
	}
	else
	{
		Health += Health + HealValue;
	}
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);
	PlayerCharacter->SetHealth(Health);
	Destroy();
}


#include "AmmoBox.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.h"
#include "WeaponType.h"

AAmmoBox::AAmmoBox()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	StaticMeshCompoent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshCompoent->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereComponent->SetupAttachment(RootComponent);

	SphereComponent->SetCollisionProfileName("OverlapAllDynamic");
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAmmoBox::OnOverlap);
}

void AAmmoBox::BeginPlay()
{
	Super::BeginPlay();
	BobbingStartpos = GetActorLocation().Z;
	BobbingHeight = 20.0f;
	GetWorldTimerManager().SetTimer(BobbingTimerHandle, this, &AAmmoBox::Bobbing, 0.03f, true);
}

void AAmmoBox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(BobbingTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AAmmoBox::Bobbing()
{
	float CurrentTime = GetWorld()->GetRealTimeSeconds();
	FVector NewActorLocation = GetActorLocation();
	float BobbingOffset = (FMath::Sin(CurrentTime) + 1.0f) * 0.5f;
	NewActorLocation.Z = BobbingStartpos + (BobbingHeight * BobbingOffset);
	SetActorLocation(NewActorLocation);
}

void AAmmoBox::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	APawn* PawnActor = Cast<APawn>(OtherActor);
	if (!PawnActor) return;

	ACharacter* Character = Cast<ACharacter>(PawnActor);
	if (!Character) return;

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Character);
	if (!PlayerCharacter) return;

	PlayerCharacter->AddAmmo(EWeaponType::WT_Pistol, 999);
	PlayerCharacter->AddAmmo(EWeaponType::WT_Rifle, 999);
	PlayerCharacter->AddAmmo(EWeaponType::WT_Shotgun, 999);
	Destroy();
}


#include "PickupItem.h"
#include "Components/SphereComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Inventory.h"
#include "MyGameInstance.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	StaticMeshCompoent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshCompoent->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereComponent->SetupAttachment(RootComponent);

	SphereComponent->SetCollisionProfileName("OverlapAllDynamic");
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupItem::OnOverlap);

	BobbingStartpos = 0.0f;
	BobbingHeight = 0.0f;
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	BobbingStartpos = GetActorLocation().Z;
	BobbingHeight = 20.0f;

	GetWorldTimerManager().SetTimer(BobbingTimerHandle, this, &APickupItem::Bobbing, 0.03f, true);
}

void APickupItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(BobbingTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void APickupItem::Bobbing()
{
	float CurrentTime = GetWorld()->GetRealTimeSeconds();
	FVector NewActorLocation = GetActorLocation();
	float BobbingOffset = (FMath::Sin(CurrentTime) + 1.0f) * 0.5f;
	NewActorLocation.Z = BobbingStartpos + (BobbingHeight * BobbingOffset);
	SetActorLocation(NewActorLocation);

}

void APickupItem::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	APawn* PawnActor = Cast<APawn>(OtherActor);
	if (!PawnActor) return;

	//ACharacter* Character = Cast<ACharacter>(PawnActor);
	//if (!Character) return;

	if (!Item) return;

	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (!GameInstance) return;
	GameInstance->Inventory->AddItem(Item);
	Destroy();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("BasicPistol"));
}
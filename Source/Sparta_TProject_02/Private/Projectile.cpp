#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.f);
	CollisionComp->SetCollisionProfileName("BlockAllDynamic");
	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

    if (GetOwner())
    {
        CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
    }

    AController* InstigatorController = GetInstigatorController();

    if (!InstigatorController && GetOwner())
    {
        if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
        {
            InstigatorController = OwnerPawn->GetController();
        }
    }

    if (!InstigatorController)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            InstigatorController = PC;
        }
    }
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
        AController* InstigatorController = GetInstigatorController();

        if (!InstigatorController && GetOwner())
        {
            if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
                InstigatorController = OwnerPawn->GetController();
        }
        if (!InstigatorController)
        {
            if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
            {
                InstigatorController = PC;
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Projectile hit %s (Controller: %s)"),
            *OtherActor->GetName(),
            *GetNameSafe(InstigatorController));

        UGameplayStatics::ApplyDamage(
            OtherActor,
            Damage,
            InstigatorController,
            this,
            UDamageType::StaticClass()
        );

        Destroy();
    }
}
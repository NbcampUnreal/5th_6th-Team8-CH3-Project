#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadeActor.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API AGrenadeActor : public AActor
{
    GENERATED_BODY()

public:
    AGrenadeActor();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    float FuseTime = 3.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    float ExplosionRadius = 300.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    float ExplosionDamage = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    class UParticleSystem* ExplosionEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    class USoundBase* ExplosionSound;

private:
    FTimerHandle FuseTimerHandle;

    void Explode();
};

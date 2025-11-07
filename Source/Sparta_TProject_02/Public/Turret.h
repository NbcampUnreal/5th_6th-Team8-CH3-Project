#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UParticleSystem;
class USoundBase;
class AProjectile;
class AAIMonsterBase;


UCLASS()
class SPARTA_TPROJECT_02_API ATurret : public AActor
{
	GENERATED_BODY()

public:
	ATurret();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* DetectionSphere;

	UPROPERTY(EditAnywhere, Category = "Turret|Stats")
	float RotationSpeed = 60.f;

	UPROPERTY(EditAnywhere, Category = "Turret|Stats")
	float FireInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Turret|Stats")
	float LifeTime = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Turret|Stats")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Turret|Stats")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* MuzzleFX;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* SpawnSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* DestroyEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* DestroySound;



	UPROPERTY(EditAnywhere, Category = "Turret|Projectile")
	TSubclassOf<AProjectile> ProjectileClass;


	UPROPERTY()
	AActor* CurrentTarget;

	UPROPERTY()
	FRotator CurrentTurretRotation; // 중복 선언 제거 후 유지

	bool bHasTarget;

	FTimerHandle ScanTimerHandle;
	FTimerHandle FireTimerHandle;
	FTimerHandle LifeTimerHandle;


	void ScanForTargets();
	void RotateIdle(float DeltaTime);
	void AimAtTarget(float DeltaTime);
	void Fire();
	void DestroySelf();

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TakeTurretDamage(float Amount);

};
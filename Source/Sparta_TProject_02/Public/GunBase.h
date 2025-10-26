#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunBase.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;

UCLASS()
class SPARTA_TPROJECT_02_API AGunBase : public AActor
{
	GENERATED_BODY()

public:
	AGunBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float Damage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 MaxMagazineAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 CurrentAmmo;
	bool bIsReloading;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRate;
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;

	virtual void TraceFire();

public:
	virtual void StartFire();
	virtual void StopFire();

	void Reload();
	void FinishReload();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void Shoot();

	USkeletalMeshComponent* GetGunMesh() const { return GunMesh; }
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponType.h"
#include "GunBase.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;
class APlayerCharacter;
class USoundBase;
class UParticleSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponEvent);

UCLASS()
class SPARTA_TPROJECT_02_API AGunBase : public AActor
{
	GENERATED_BODY()

public:
	AGunBase();

	virtual void BeginPlay() override;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponEvent OnStartFire;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponEvent OnStopFire;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponEvent OnStartReload;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponEvent OnFinishReload;

	// API
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsReloading() const { return bIsReloading; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponType GetWeaponType() const { return WeaponType; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void FinishReload();

	// Getter montages for external (Player) to play on FP mesh
	UFUNCTION(BlueprintPure, Category = "Weapon")
	UAnimMontage* GetFireMontage() const { return FireMontage; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UAnimMontage* GetReloadMontage() const { return ReloadMontage; }

	// hide/show
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponHidden(bool bShouldBeHidden);

	// Get current ammo for HUD
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetMaxMagazineAmmo() const { return MaxMagazineAmmo; }

protected:
	// perform actual trace / damage; can be overridden by derived classes (shotgun)
	virtual void TraceFire();

	// Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USkeletalMeshComponent* GunMesh;

	// Stats
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 MaxMagazineAmmo;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRate; // RPM or 0 for single-shot

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	EWeaponType WeaponType;

	// Effects
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Effects")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Effects")
	USoundBase* FireSound;

	// Animation montages (BP에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Animation")
	UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Animation")
	UAnimMontage* ReloadMontage;

	// Timer handles
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;

	// Cached owner player
	UPROPERTY()
	APlayerCharacter* OwningPlayer;

	bool bIsReloading;
};
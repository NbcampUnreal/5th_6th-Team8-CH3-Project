#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponType.h"
#include "GrenadeActor.h"
#include "PlayerCharacter.generated.h"

class UInputAction;
class USkeletalMeshComponent;
class UCameraComponent;
class AGunBase;
struct FInputActionValue;

UCLASS()
class SPARTA_TPROJECT_02_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	//GameMode ȣ��
	void HealOnWaveClear(float HealAmount);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FP_Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* PlayerMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ShootAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ReloadAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* EquipShotgunAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* EquipRifleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* EquipPistolAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* GrenadeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float NormalSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeedMultiplier;

	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	TArray<TSubclassOf<AGunBase>> StartWeaponClasses;

	UPROPERTY(VisibleInstanceOnly, Category = "Gun")
	TArray<AGunBase*> Weapons;

	UPROPERTY(VisibleInstanceOnly, Category = "Gun")
	AGunBase* CurrentWeapon;

	int32 CurrentWeaponIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	TMap<EWeaponType, int32> AmmoReserve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	TMap<EWeaponType, int32> MaxCarryAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
	TSubclassOf<AGrenadeActor> GrenadeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float GrenadeThrowStrength = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float GrenadeCooldown = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
	USoundBase* ThrowGrenadeSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
	UAnimMontage* ThrowGrenadeMontage;

	bool bCanThrowGrenade = true;
	FTimerHandle GrenadeCooldownHandle;

	UFUNCTION()
	void ThrowGrenade(const FInputActionValue& Value);

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AddAmmo(EWeaponType WeaponType, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 ConsumeAmmoForReload(EWeaponType WeaponType, int32 RequestedAmount);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetReserveAmmo(EWeaponType WeaponType) const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	AGunBase* GetCurrentWeapon() const { return CurrentWeapon; }

	bool IsSprinting() const { return bWantsToSprint; }
	bool IsAiming() const { return bIsAiming; }
	bool IsFiring() const { return bIsFiring; } 

protected:
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void StartJump(const FInputActionValue& value);
	void StopJump(const FInputActionValue& value);
	void StartSprint(const FInputActionValue& value);
	void StopSprint(const FInputActionValue& value);
	void StartCrouch(const FInputActionValue& value);
	void StopCrouch(const FInputActionValue& value);
	void StartShoot(const FInputActionValue& value);
	void StopShoot(const FInputActionValue& value);
	void StartReload(const FInputActionValue& value);
	void EquipShotgun(const FInputActionValue& value);
	void EquipRifle(const FInputActionValue& value);
	void EquipPistol(const FInputActionValue& value);

	void EquipWeaponByType(EWeaponType TypeToEquip);
	void EquipWeapon(int32 Index);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Health;
	UFUNCTION()
	void OnWeaponStartFire();

	UFUNCTION()
	void OnWeaponStopFire();

	UFUNCTION()
	void OnWeaponStartReload();

	UFUNCTION()
	void OnWeaponFinishReload();

private:
	bool bWantsToSprint;
	bool bIsAiming;
	bool bIsFiring;

public:
	UFUNCTION(BlueprintPure, Category = "Mesh")
	USkeletalMeshComponent* GetFPMesh() const { return FP_Mesh; };

};

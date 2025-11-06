#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponType.h"
#include "GrenadeActor.h"
#include "STPlayerState.h"
#include "Delegates/Delegate.h"
#include "PlayerCharacter.generated.h"

class UInputAction;
class USkeletalMeshComponent;
class UCameraComponent;
class AGunBase;
struct FInputActionValue;
class AShop;

// 🔸 Hit 이벤트용 델리게이트 정의
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyHitSignature);

UCLASS()
class SPARTA_TPROJECT_02_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	// --- GameMode 호출용 ---
	void HealOnWaveClear(float HealAmount);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- Camera & Mesh ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FP_Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* PlayerMesh;

	// --- Input Actions ---
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InventoryAction;

	// --- Movement ---
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BaseSpeed;
	float NormalSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeedMultiplier;
	float SprintSpeed;

	// --- Weapons ---
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

	// --- Grenade ---
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
	// --- Weapon 관련 함수 ---
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
	// --- Shop 관련 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	AShop* ShopActor;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void OpenShop();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void CloseShop();

protected:
	// --- Input 처리 함수 ---
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

	void ToggleInventory(const FInputActionValue& value);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 BaseDefense;
	int32 Defense;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Attack_Increase;

	//float GetMaxHealth() const;
	//float GetHealth() const;
	//float GetBaseDefense() const;
	//float GetDefense() const;
	//float GetAttack_Increase() const;

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

	// --- HUD 델리게이트 ---
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmoChangedSignature OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyHitSignature OnEnemyHit;

	// Gem 장착으로 인한 스탯변화를 계산하는 함수
	bool CalculateStats();
protected:
	UFUNCTION()
	void HandleHealthChanged(float Current, float Max)
	{
		OnHealthChanged.Broadcast(Current, Max);
	}

	UFUNCTION()
	void HandleAmmoChanged(int32 Current, int32 Max)
	{
		OnAmmoChanged.Broadcast(Current, Max);
	}

	UFUNCTION()
	void HandleKillCountChanged(int32 Kills)
	{
		// 필요시 UI용 델리게이트 추가
	}
};

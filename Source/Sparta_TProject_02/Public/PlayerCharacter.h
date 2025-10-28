#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PlayerCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class AGunBase;
enum class EWeaponType : uint8; 

UCLASS()
class SPARTA_TPROJECT_02_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FP_Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* PlayerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* ShootAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* ReloadAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* NextWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* PrevWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float NormalSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float SprintSpeedMultiplier;

	float SprintSpeed;

	// ����: ���� �ѱ� Ŭ�������� ���� ���� Ŭ���� �迭�� ����
	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	TArray<TSubclassOf<AGunBase>> StartWeaponClasses;

	UPROPERTY(VisibleInstanceOnly, Category = "Gun")
	AGunBase* CurrentWeapon;

	// �߰�: ������ ��� ���⸦ ������ �迭
	UPROPERTY(VisibleInstanceOnly, Category = "Gun")
	TArray<AGunBase*> Weapons;

	// �߰�: ���� ��� �ִ� ������ �ε���
	int32 CurrentWeaponIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	TMap<EWeaponType, int32> AmmoReserve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	TMap<EWeaponType, int32> MaxCarryAmmo;

	// ����: AGunBase���� �����ؾ� �ϹǷ� protected���� public���� �̵��մϴ�.
public:
	// ź�� �߰� (������ ȹ��, ���� ���� ��)
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AddAmmo(EWeaponType WeaponType, int32 Amount);

	// ź�� ��� (������ ��)
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 ConsumeAmmoForReload(EWeaponType WeaponType, int32 RequestedAmount);

	// ���� ����ź�� ��������
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetReserveAmmo(EWeaponType WeaponType) const;

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

	// �߰�: ���� ��ü �Լ�
	void NextWeapon(const FInputActionValue& value);
	void PrevWeapon(const FInputActionValue& value);
	void EquipWeapon(int32 Index);

public:
	USkeletalMeshComponent* GetPlayerMesh() const { return PlayerMesh; }
	UCameraComponent* GetCameraComp() const { return CameraComp; }
	USkeletalMeshComponent* GetFPMesh() const { return FP_Mesh; }
};
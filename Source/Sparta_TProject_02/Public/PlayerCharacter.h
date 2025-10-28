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

	// 변경: 단일 총기 클래스에서 시작 무기 클래스 배열로 변경
	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	TArray<TSubclassOf<AGunBase>> StartWeaponClasses;

	UPROPERTY(VisibleInstanceOnly, Category = "Gun")
	AGunBase* CurrentWeapon;

	// 추가: 스폰된 모든 무기를 보관할 배열
	UPROPERTY(VisibleInstanceOnly, Category = "Gun")
	TArray<AGunBase*> Weapons;

	// 추가: 현재 들고 있는 무기의 인덱스
	int32 CurrentWeaponIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	TMap<EWeaponType, int32> AmmoReserve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	TMap<EWeaponType, int32> MaxCarryAmmo;

	// 수정: AGunBase에서 접근해야 하므로 protected에서 public으로 이동합니다.
public:
	// 탄약 추가 (아이템 획득, 상점 구매 등)
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AddAmmo(EWeaponType WeaponType, int32 Amount);

	// 탄약 사용 (재장전 시)
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 ConsumeAmmoForReload(EWeaponType WeaponType, int32 RequestedAmount);

	// 현재 예비탄약 가져오기
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

	// 추가: 무기 교체 함수
	void NextWeapon(const FInputActionValue& value);
	void PrevWeapon(const FInputActionValue& value);
	void EquipWeapon(int32 Index);

public:
	USkeletalMeshComponent* GetPlayerMesh() const { return PlayerMesh; }
	UCameraComponent* GetCameraComp() const { return CameraComp; }
	USkeletalMeshComponent* GetFPMesh() const { return FP_Mesh; }
};
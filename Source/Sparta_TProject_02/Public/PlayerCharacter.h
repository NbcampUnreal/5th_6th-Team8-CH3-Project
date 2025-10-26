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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float NormalSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float SprintSpeedMultiplier;

	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	TSubclassOf<AGunBase> DefaultGunClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Gun")
	TObjectPtr<AGunBase> CurrentGun;

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

public:
	USkeletalMeshComponent* GetPlayerMesh() const { return PlayerMesh; }
	UCameraComponent* GetCameraComp() const { return CameraComp; }
	USkeletalMeshComponent* GetFPMesh() const { return FP_Mesh; }
};
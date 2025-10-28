#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool bIsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool bIsReloading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool bIsFiring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	EWeaponType CurrentWeaponType;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	APawn* OwningPawn;
};
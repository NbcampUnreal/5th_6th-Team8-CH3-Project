#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TurretAnimInstance.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API UTurretAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret Animation")
	FRotator HeadRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret Animation")
	float BarrelOffset;

};
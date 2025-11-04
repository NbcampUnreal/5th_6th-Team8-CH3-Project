#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "Shotgun.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API AShotgun : public AGunBase
{
	GENERATED_BODY()

public:
	AShotgun();

protected:
	virtual void TraceFire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float SpreadAngle;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 PelletsPerShot;
};
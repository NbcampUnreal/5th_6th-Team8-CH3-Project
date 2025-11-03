#pragma once
#include "CoreMinimal.h"
#include "Gem.h"
#include "SpeedGem.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API USpeedGem : public UGem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float SpeedValue;

public:
	USpeedGem();
	float GetSpeedValue();
};

#pragma once
#include "CoreMinimal.h"
#include "Gem.h"
#include "DefenseGem.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API UDefenseGem : public UGem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 DefenseValue;

public:
	UDefenseGem();
	int32 GetDefenseValue();
};

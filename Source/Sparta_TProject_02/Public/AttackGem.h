#pragma once
#include "CoreMinimal.h"
#include "Gem.h"
#include "AttackGem.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API UAttackGem : public UGem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 AttackValue;
	
public:
	UAttackGem();
	int32 GetAttackValue();
};

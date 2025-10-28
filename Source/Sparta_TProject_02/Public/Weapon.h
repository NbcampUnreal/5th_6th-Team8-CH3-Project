#pragma once
#include "CoreMinimal.h"
#include "EquipmentItem.h"
#include "Weapon.generated.h"

UCLASS()
class UWeapon : public UEquipmentItem
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int Damage;
public:
	UWeapon();
};

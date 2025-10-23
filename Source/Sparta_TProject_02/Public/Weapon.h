#pragma once
#include "CoreMinimal.h"
#include "EquipmentItem.h"
#include "Weapon.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API AWeapon : public AEquipmentItem
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int damage;
public:
	AWeapon();
};

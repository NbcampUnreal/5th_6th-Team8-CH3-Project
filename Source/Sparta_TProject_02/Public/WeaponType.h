#pragma once

#include "CoreMinimal.h"
#include "WeaponType.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    WT_None     UMETA(DisplayName = "None"),
    WT_Pistol   UMETA(DisplayName = "Pistol"),
    WT_Rifle    UMETA(DisplayName = "Rifle"),
    WT_Shotgun  UMETA(DisplayName = "Shotgun")
};
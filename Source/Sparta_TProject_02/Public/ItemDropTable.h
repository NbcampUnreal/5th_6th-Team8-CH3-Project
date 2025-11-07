// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupItem.h"
#include "ItemDropTable.generated.h"

USTRUCT(BlueprintType)
struct FItemDropTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Chance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CumulativeChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APickupItem> DropItemBlueprint;
};
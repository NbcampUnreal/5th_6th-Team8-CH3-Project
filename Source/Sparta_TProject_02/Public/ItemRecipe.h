#pragma once

#include "CoreMinimal.h"
#include "ItemRecipe.generated.h"

USTRUCT(BlueprintType)
struct FRecipeItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity;

	FRecipeItem() : ItemName(NAME_None), Quantity(0) {}
};

USTRUCT(BlueprintType)
struct FRecipe
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRecipeItem> Requirements;
};
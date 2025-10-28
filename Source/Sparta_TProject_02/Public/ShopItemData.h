// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShopItemData.generated.h"

USTRUCT(BlueprintType)
struct FRecipeIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopItemData")
	FName ItemID = FName(TEXT("Default"));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopItemData")
	int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopItemData")
	FName ItemID = FName(TEXT("Item_Default"));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopItemData")
	FText ItemName = FText::FromString(TEXT("New Item"));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopItemData")
	FName ItemType = FName(TEXT("Item_Default"));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopItemData")
	FText FullDescription = FText::FromString(TEXT("Default Description."));
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopItemData")
	class UTexture2D* IconTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShopItemData")
	TArray<FRecipeIngredient> Recipe;
};
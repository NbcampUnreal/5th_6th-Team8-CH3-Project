#pragma once

#include "CoreMinimal.h"
#include "ItemRecipe.h"
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
struct FShopItemData
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCraftable = false;	//Buy 아이템인지, Craft 아이템인지 분별용 bool

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRecipe CraftingRecipe;		//제작에 필요한 재료 목록
};
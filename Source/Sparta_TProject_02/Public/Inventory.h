#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/NoExportTypes.h"
#include "ItemRecipe.h"
#include "Inventory.generated.h"

class UItem;
class UInventoryWidgetBase;

UCLASS(Blueprintable, BlueprintType)
class SPARTA_TPROJECT_02_API UInventory : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	TArray<UItem*> ItemArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	UInventoryWidgetBase* LinkedWidget;

	int32 MaxSize;

public:
	UInventory();
	TArray<UItem*> GetInventory() const;
	UUserWidget* GetLinkedWidget() const;
	bool SetLinkedWidget(UInventoryWidgetBase* InLinkedWidget);
	UItem* GetItem(int32 index) const;
	int32 GetCurrentSize() const;
	int32 GetMaxSize() const;

	bool IsValidIdx(int32 Index) const;
	bool IsEmpty() const;
	
	bool SetMaxSize(int32 NewMaxSize);
	bool AddItem(UItem* item);
	bool RemoveItemIndex(int32 Index);

	TArray<int32> FindItemName(FName ItemName);
	TArray<int32> FindItemType(FName ItemType);
	bool Swap(int32 Index1, int32 Index2);


	//ShopUI
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TMap<FName, TSoftObjectPtr<UItem>> ItemAssetMap;

	bool TryCraftItem(FName ResultITemName, const FRecipe& Recipe);
	int32 GetItemQuantity(FName ItemName) const;
	bool RemoveItemQuantity(FName ItemName, int32 Quantity);
	UItem* LoadItemByFName(FName ItemName);

};

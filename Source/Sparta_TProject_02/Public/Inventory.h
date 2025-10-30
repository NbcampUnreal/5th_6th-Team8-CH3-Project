#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory.generated.h"
class UItem;

UCLASS()
class SPARTA_TPROJECT_02_API UInventory : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	TArray<UItem*> ItemArray;

	int32 MaxSize;

public:
	UInventory();
	//static UInventory* GetInstance() {
	//	static UInventory* instance;
	//	return instance;
	//}
	TArray<UItem*> GetInventory() const;
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
};

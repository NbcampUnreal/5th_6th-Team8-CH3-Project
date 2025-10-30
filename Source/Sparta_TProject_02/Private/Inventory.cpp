#include "Inventory.h"
#include "Item.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

UInventory::UInventory()
{
	MaxSize = 16;
}

TArray<UItem*> UInventory::GetInventory() const
{
	return ItemArray;
}

UItem* UInventory::GetItem(int32 index) const
{
	return ItemArray[index];
}

int32 UInventory::GetCurrentSize() const
{
	return ItemArray.Num();
}

int32 UInventory::GetMaxSize() const
{
	return MaxSize;
}

bool UInventory::IsValidIdx(int32 Index) const
{
	return ItemArray.IsValidIndex(Index);
}

bool UInventory::IsEmpty() const
{
	return ItemArray.IsEmpty();
}

bool UInventory::SetMaxSize(int32 NewMaxSize)
{
	MaxSize = NewMaxSize;
	return true;
}

bool UInventory::AddItem(UItem* Item)
{
	int32 Size = ItemArray.Num();
	if (Size >= MaxSize) return false;

	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
	
	if (!GameInstance) return false;
	
	GameInstance->AddItemButton(Item);

	ItemArray.Add(Item);
	++Size;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
		FString::Printf(TEXT("%d / %d"), Size, MaxSize));
	return true;
}

bool UInventory::RemoveItemIndex(int32 Index)
{
	int32 Size = ItemArray.Num();
	if (Size <= Index) return false;

	ItemArray.RemoveAt(Index);
	return true;
}

TArray<int32> UInventory::FindItemName(FName ItemName)
{
	TArray<int32> Found;
	for (int32 i = 0; i < ItemArray.Num(); ++i)
	{
		if (ItemArray[i]->GetItemName() == ItemName)
		{
			Found.Add(i);
		}
	}
	return Found;
}

TArray<int32> UInventory::FindItemType(FName ItemType)
{
	TArray<int32> Found;
	for (int32 i = 0; i < ItemArray.Num(); ++i)
	{
		if (ItemArray[i]->GetItemType() == ItemType)
		{
			Found.Add(i);
		}
	}
	return Found;
}

bool UInventory::Swap(int32 Index1, int32 Index2)
{
	if (!(IsValidIdx(Index1) && IsValidIdx(Index2))) return false;
	ItemArray.Swap(Index1, Index2);
	return true;
}
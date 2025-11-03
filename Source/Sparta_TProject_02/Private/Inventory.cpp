#include "Inventory.h"
#include "Item.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

//ShopUI
#include "MaterialItem.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/NoExportTypes.h"

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


//ShopUI
int32 UInventory::GetItemQuantity(FName ItemName) const
{
	int32 Count = 0;

	for (UItem* Item : ItemArray)
	{
		if (Item->GetItemName() == ItemName)
		{
			// 찾는 아이템 대상이 UMaterialItem 일 시 현재 보유 개수 (Stack)을 가져옴
			if (class UMaterialItem* MaterialItem = Cast<UMaterialItem>(Item))
			{
				Count += MaterialItem->GetCurrentStack();
			}
			// UMaterialItem이 아닐 시, 스택되는 아이템이 아니기에 1개만 리턴함
			else
			{
				Count += 1;
			}
		}
	}
	return Count;
}

bool UInventory::RemoveItemQuantity(FName ItemName, int32 Quantity)
{
	if (GetItemQuantity(ItemName) < Quantity)
	{
		return false;
	}

	int32 QuantityToRemove = Quantity;

	for (int32 i = ItemArray.Num() - 1; i >= 0 && QuantityToRemove > 0; --i)
	{
		UItem* CurrentItem = ItemArray[i];
		if (CurrentItem && CurrentItem->GetItemName() == ItemName)
		{
			// CurrentItem이 UMaterialItem인 경우
			if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(CurrentItem))
			{
				int32 CurrentStack = MaterialItem->GetCurrentStack();

				if (CurrentStack <= QuantityToRemove)
				{
					QuantityToRemove -= CurrentStack;
					ItemArray.RemoveAt(i);
				}
				else
				{
					MaterialItem->SetCurrentStack(CurrentStack - QuantityToRemove);
					QuantityToRemove = 0;
				}
			}
			// CurrentItem이 UMaterialItem이 아닌 경우
			else
			{
				QuantityToRemove -= 1;
				ItemArray.RemoveAt(i);
			}
		}
	}
	return QuantityToRemove == 0;
}

bool UInventory::TryCraftItem(FName ResultItemName, const FRecipe& Recipe)
{
	// 현재 재료가 충분한지 확인
	for (const FRecipeItem& Requirement : Recipe.Requirements)
	{
		if (GetItemQuantity(Requirement.ItemName) < Requirement.Quantity)
		{
			// 현재 재료의 개수가 필요량 보다 적을 시 false 리턴;
			return false;
		}
	}

	// 재료가 충분한 경우로 왔으니, 재료를 인벤토리에서 제거
	for (const FRecipeItem& Requirement : Recipe.Requirements)
	{
		if (!RemoveItemQuantity(Requirement.ItemName, Requirement.Quantity)) // 여기 조건문에서 이미 재료를 제거하는 함수를 실행함
		{
			// 이미 충분한 재료가 인벤토리에 있다는 걸 확인 하고 넘어왔는데, false는 재료 불충분 시 리턴 됨. 즉 크리티컬 에러.
			UE_LOG(LogTemp, Error, TEXT("Critical Error: Failed to remove materials during crafting."));
			return false;
		}
	}

	// 제작 완성 아이템 인벤토리에 추가
	UItem* CraftedItem = LoadItemByFName(ResultItemName);

	if (CraftedItem)
	{
		AddItem(CraftedItem);
		return true;
	}

	return false;
}

UItem* UInventory::LoadItemByFName(FName ItemName)
{
	if (!ItemAssetMap.Contains(ItemName))
	{
		return nullptr;
	}

	TSoftObjectPtr<UItem> ItemSoftPtr = ItemAssetMap[ItemName];
	UItem* OriginalAsset = ItemSoftPtr.LoadSynchronous();

	if (!OriginalAsset)
	{
		//Asset 파일 로드 실패
		return nullptr;
	}

	UItem* NewItemInstance = DuplicateObject<UItem>(OriginalAsset, this);

	return NewItemInstance;
}
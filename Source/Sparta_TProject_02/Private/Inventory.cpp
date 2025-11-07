#include "Inventory.h"
#include "Item.h"
#include "MaterialItem.h"
#include "MyGameInstance.h"
#include "InventoryWidget.h"
#include "EquipmentWidget.h"
#include "Kismet/GameplayStatics.h"

//ShopUI
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

	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance) return false;

	if (UMaterialItem* StackableItem = Cast<UMaterialItem>(Item))
	{
		if (StackableItem->IsStackable() && StackableItem->GetItemMaxStack() > 1)
		{
			FName ItemName = Item->GetItemName();
			bool bSuccess = false;
			for (UItem* Element : ItemArray)
			{
				if (Element->GetItemName() == ItemName)
				{
					if (UMaterialItem* MTElement = Cast<UMaterialItem>(Element))
					{
						if (MTElement->SetItemStack(MTElement->GetItemCurrentStack() + 1)) 
						{
							bSuccess = true;
							break;
						}
					}
				}
			}
			if (bSuccess)
			{
				GameInstance->GetInventoryWidget()->RefreshWidget();
				GameInstance->GetEquipmentWidget()->RefreshWidget();
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("%d / %d"), Size, MaxSize));
				return true;
			}
		}
	}
	if (Size >= MaxSize) return false;

	ItemArray.Add(Item);
	++Size;
	GameInstance->GetInventoryWidget()->RefreshWidget();
	GameInstance->GetEquipmentWidget()->RefreshWidget();

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("%d / %d"), Size, MaxSize));
	return true;
}

bool UInventory::RemoveItemIndex(int32 Index)
{
	int32 Size = ItemArray.Num();
	if (Size <= Index) return false;

	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance) return false;

	ItemArray.RemoveAt(Index);
	GameInstance->GetInventoryWidget()->RefreshWidget();
	GameInstance->GetEquipmentWidget()->RefreshWidget();

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
			// ã�� ������ ����� UMaterialItem �� �� ���� ���� ���� (Stack)�� ������
			if (class UMaterialItem* MaterialItem = Cast<UMaterialItem>(Item))
			{
				Count += MaterialItem->GetItemCurrentStack();
			}
			// UMaterialItem�� �ƴ� ��, ���õǴ� �������� �ƴϱ⿡ 1���� ������
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
			// CurrentItem�� UMaterialItem�� ���
			if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(CurrentItem))
			{
				int32 CurrentStack = MaterialItem->GetItemCurrentStack();

				if (CurrentStack <= QuantityToRemove)
				{
					QuantityToRemove -= CurrentStack;
					ItemArray.RemoveAt(i);
				}
				else
				{
					MaterialItem->SetItemStack(CurrentStack - QuantityToRemove);
					QuantityToRemove = 0;
				}
			}
			// CurrentItem�� UMaterialItem�� �ƴ� ���
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
	// ���� ��ᰡ ������� Ȯ��
	for (const FRecipeItem& Requirement : Recipe.Requirements)
	{
		if (GetItemQuantity(Requirement.ItemName) < Requirement.Quantity)
		{
			// ���� ����� ������ �ʿ䷮ ���� ���� �� false ����;
			return false;
		}
	}

	// ��ᰡ ����� ���� ������, ��Ḧ �κ��丮���� ����
	for (const FRecipeItem& Requirement : Recipe.Requirements)
	{
		if (!RemoveItemQuantity(Requirement.ItemName, Requirement.Quantity)) // ���� ���ǹ����� �̹� ��Ḧ �����ϴ� �Լ��� ������
		{
			// �̹� ����� ��ᰡ �κ��丮�� �ִٴ� �� Ȯ�� �ϰ� �Ѿ�Դµ�, false�� ��� ����� �� ���� ��. �� ũ��Ƽ�� ����.
			UE_LOG(LogTemp, Error, TEXT("Critical Error: Failed to remove materials during crafting."));
			return false;
		}
	}

	// ���� �ϼ� ������ �κ��丮�� �߰�
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
		//Asset ���� �ε� ����
		return nullptr;
	}

	UItem* NewItemInstance = DuplicateObject<UItem>(OriginalAsset, this);

	return NewItemInstance;
}
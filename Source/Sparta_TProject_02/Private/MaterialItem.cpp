#include "MaterialItem.h"

UMaterialItem::UMaterialItem()
{
	CurrentStack = 1;
	MaxStack = 1;
	ItemType = "Material";
}

int32 UMaterialItem::GetItemCurrentStack()
{
	return CurrentStack;
}

int32 UMaterialItem::GetItemMaxStack()
{
	return MaxStack;
}

bool UMaterialItem::SetItemStack(int32 Value)
{
	if (Value <= MaxStack)
	{
		CurrentStack = Value;
		return true;
	}
	return false;
}

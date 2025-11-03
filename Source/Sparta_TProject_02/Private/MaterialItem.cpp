#include "MaterialItem.h"

UMaterialItem::UMaterialItem()
{
	CurrentStack = 1;
	MaxStack = 1;
	ItemType = "Material";
}

int32 UMaterialItem::GetCurrentStack()
{
	return CurrentStack;
}

int32 UMaterialItem::GetItemMaxStack()
{
	return MaxStack;
}

void UMaterialItem::SetCurrentStack(int32 NewStackAmount)
{
	CurrentStack = NewStackAmount;
}
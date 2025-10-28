#include "Item.h"

UItem::UItem()
{
	ItemName = "";
	ItemType = "Item";
	ItemPrice = 1;
	bIsStackable = false;
}

FName UItem::GetItemName()
{
	return ItemName;
}

FName UItem::GetItemType()
{
	return ItemType;
}

int32 UItem::GetItemPrice()
{
	return ItemPrice;
}

bool UItem::IsStackable()
{
	return bIsStackable;
}

void UItem::Use()
{

}
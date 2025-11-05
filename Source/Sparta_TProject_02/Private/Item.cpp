#include "Item.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

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

UTexture2D* UItem::GetItemIcon()
{
	if (ItemIcon.IsValid())
	{
		return ItemIcon.Get();
	}
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	UTexture2D* LoadedTexture = Cast<UTexture2D>(Streamable.LoadSynchronous(ItemIcon.ToSoftObjectPath(), true));
	return LoadedTexture;
}
bool UItem::IsStackable()
{
	return bIsStackable;
}

void UItem::Use()
{

}


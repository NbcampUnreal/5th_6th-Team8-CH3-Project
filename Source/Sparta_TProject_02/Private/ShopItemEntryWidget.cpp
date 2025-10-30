// ShopItemEntryWidget.cpp

#include "ShopItemEntryWidget.h"

void UShopItemEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (ItemButton)
    {
        // Hover 이벤트에 C++ 함수 바인딩
        ItemButton->OnHovered.AddDynamic(this, &UShopItemEntryWidget::HandleItemButtonHovered);
    }

    if (CraftButton)
    {
        CraftButton->OnClicked.AddDynamic(this, &UShopItemEntryWidget::HandleCraftButtonClicked);
    }
}

void UShopItemEntryWidget::HandleItemButtonHovered()
{
    OnItemHovered.Broadcast(ShopItemData);
}

void UShopItemEntryWidget::HandleCraftButtonClicked()
{
    OnItemCrafted.Broadcast(ShopItemData);
}
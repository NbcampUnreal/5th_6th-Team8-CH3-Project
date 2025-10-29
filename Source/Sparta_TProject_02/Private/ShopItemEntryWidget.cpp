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
}

void UShopItemEntryWidget::HandleItemButtonHovered()
{
    OnItemHovered.Broadcast(ShopItemData);
}
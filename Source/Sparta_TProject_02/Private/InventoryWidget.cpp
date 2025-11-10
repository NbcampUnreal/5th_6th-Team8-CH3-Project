#include "InventoryWidget.h"
#include "Item.h"
#include "MaterialItem.h"
#include "Inventory.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateBrush.h"
#include "Layout/Margin.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ItemButtonWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "MyGameInstance.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
   GridMaxColumn = 4;
}

void UInventoryWidget::NativeConstruct()
{
   Super::NativeConstruct();
   PlayerContlloer = GetOwningPlayer();
   InventoryGrid = Cast<UGridPanel>(GetWidgetFromName("GridPanel"));

   UBorder* Border = Cast<UBorder>(GetWidgetFromName(TEXT("ItemContextMenuBorder")));
   if (!Border) return;

   UseItemButtonWidget = Cast<UItemButtonWidget>(GetWidgetFromName("UseButton"));
   if (!UseItemButtonWidget) return;

   UseItemButtonWidget->SetupClickBinding();
   //UseItemButtonWidget->OnClicked.AddDynamic(this, &UInventoryWidget::ItemTooltipShow);
   UseItemButtonWidget->SetButtonData({ 0, FVector2D::ZeroVector });

   DestroyItemButtonWidget = Cast<UItemButtonWidget>(GetWidgetFromName("DestroyButton"));
   if (!DestroyItemButtonWidget) return;

   DestroyItemButtonWidget->SetupClickBinding();
   DestroyItemButtonWidget->OnClicked.AddDynamic(this, &UInventoryWidget::ItemDestory);
   DestroyItemButtonWidget->SetButtonData({ 0, FVector2D::ZeroVector });
}

void UInventoryWidget::SetupWidget()
{
   UMyGameInstance* GameInstance = Cast<UMyGameInstance>(PlayerContlloer->GetGameInstance());
   if (GameInstance)
   {
      Inventory = GameInstance->GetInventory();
   }
   else
   {
      //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("InventorySet Fail"));
   }

}
bool UInventoryWidget::RefreshWidget()
{
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("RefreshWidget"));
   if (GetVisibility() == ESlateVisibility::Collapsed) return false;

   for (int32 i = InventoryGrid->GetChildrenCount(); i > 0; --i)
   {
      InventoryGrid->RemoveChildAt(0);
      //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("InventoryGrid Remove"));
   }

   int32 ItemSize = Inventory->GetCurrentSize();
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, FString::Printf(TEXT("ItemSize: %d"), ItemSize));
   for (int32 i = 0; i < ItemSize; ++i)
   {
      if (!AddItemToGrid(Inventory->GetItem(i), i))
      {
         //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("AddItemToGrid Error"));
         return false;
      }
   }
   return true;
}

bool UInventoryWidget::AddItemToGrid(UItem* Item, int32 Index)
{
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("AddItemToGrid"));
   int32 Row = Index / GridMaxColumn;
   int32 Column = Index % GridMaxColumn;
   FVector2D ItemOverlaySize = FVector2D(100.0f, 100.0f);
   FMargin ItemMargin = FMargin(5.0f, 5.0f, 0.0f, 0.0f);

   UBorder* Border = Cast<UBorder>(Cast<UWidget>(InventoryGrid)->GetParent());
   if (!Border) return false;

   UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Border);
   if (!CanvasPanelSlot) return false;

   FItemButtonData ButtonData;
   ButtonData.Index = Index;
   ButtonData.ButtonPosition = CanvasPanelSlot->GetPosition();
   ButtonData.ButtonPosition.X += (Column * ItemOverlaySize.X) + ((Column + 1) * ItemMargin.Left);
   ButtonData.ButtonPosition.Y += (Row * ItemOverlaySize.Y) + ((Row + 1) * ItemMargin.Top);

   UOverlay* ItemOverlay = CreateItemOverlay(Item, ItemOverlaySize, ButtonData);
   UGridSlot* GridSlot = InventoryGrid->AddChildToGrid(ItemOverlay);

   GridSlot->SetPadding(ItemMargin);
   GridSlot->SetRow(Row);
   GridSlot->SetColumn(Column);

   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Postion X: %f Y: %f"), ButtonData.ButtonPosition.X, ButtonData.ButtonPosition.Y));
   
   return true;
}

UOverlay* UInventoryWidget::CreateItemOverlay(UItem* Item, const FVector2D& ItemOverlaySize, FItemButtonData ButtonData)
{
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("CreateItemOverlay"));
   UOverlay* ItemOverlay = NewObject<UOverlay>(PlayerContlloer);
   if (!ItemOverlay) return nullptr;

   UItemButtonWidget* ItemButtonWidget = CreateItemButton(Item, ItemOverlay, ItemOverlaySize, ButtonData);
   if (!ItemButtonWidget) return nullptr;
   ItemOverlay->AddChild(Cast<UWidget>(ItemButtonWidget));

   UImage* Image = CreateItemImage(Item, ItemOverlay);
   if (Image)
   {
      UOverlaySlot* OverlaySlot = ItemOverlay->AddChildToOverlay(Image);
      OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
      OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
   }

   if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(Item))
   {
       UTextBlock* ItemStackTextBlock = CreateItemStackTextBlock(Item, ItemOverlay);
       if (!ItemStackTextBlock) return nullptr;
       ItemOverlay->AddChild(Cast<UWidget>(ItemStackTextBlock));

       UOverlaySlot* OverlaySlot = ItemOverlay->AddChildToOverlay(ItemStackTextBlock);
       OverlaySlot->SetPadding(FMargin(0.0f, 0.0f, 5.0f, 5.0f));
       OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
       OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
   }

   return ItemOverlay;
}

UItemButtonWidget* UInventoryWidget::CreateItemButton(UItem* Item, UOverlay* Overlay, const FVector2D ItemButtonSize, FItemButtonData ButtonData)
{
   if (!DynamicButtonClass) return nullptr;
   UItemButtonWidget* ItemButtonWidget = CreateWidget<UItemButtonWidget>(PlayerContlloer, DynamicButtonClass);
   UButton* InnerButton = ItemButtonWidget->GetButton();
   ItemButtonWidget->SetupHoverBinding();
   ItemButtonWidget->SetupClickBinding();

   FDeprecateSlateVector2D ButtonSize = FDeprecateSlateVector2D(ItemButtonSize.X, ItemButtonSize.Y);
   FButtonStyle ButtonStyle = InnerButton->GetStyle();

   FSlateBrush NormalSB = ButtonStyle.Normal;
   NormalSB.SetImageSize(ButtonSize);
   ButtonStyle.SetNormal(NormalSB);

   FSlateBrush HoveredSB = ButtonStyle.Hovered;
   HoveredSB.SetImageSize(ButtonSize);
   ButtonStyle.SetHovered(HoveredSB);

   FSlateBrush PressedSB = ButtonStyle.Pressed;
   PressedSB.SetImageSize(ButtonSize);
   ButtonStyle.SetPressed(PressedSB);

   InnerButton->SetStyle(ButtonStyle);
   ItemButtonWidget->OnHovered.AddDynamic(this, &UInventoryWidget::ItemTooltipShow);
   InnerButton->OnUnhovered.AddDynamic(this, &UInventoryWidget::ItemTooltipHide);
   ItemButtonWidget->OnClicked.AddDynamic(this, &UInventoryWidget::ItemContextMenuShow);
   ItemButtonWidget->SetButtonData(ButtonData);
   return ItemButtonWidget;
}

UTextBlock* UInventoryWidget::CreateItemStackTextBlock(UItem* Item, UOverlay* Overlay)
{
   UTextBlock* TextBlock = NewObject<UTextBlock>(Overlay);
   if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(Item))
   {
      //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("MaterialItem"));
      TextBlock->SetText(FText::FromString(FString::FromInt(MaterialItem->GetItemCurrentStack())));
   }
   else
   {
      TextBlock->SetText(FText::FromString(TEXT("1")));
   }
   return TextBlock;
}

UImage* UInventoryWidget::CreateItemImage(UItem* Item, UOverlay* Overlay)
{
   UImage* Image = NewObject<UImage>(Overlay);
   UTexture2D* ItemIcon = Item->GetItemIcon();
   if (!ItemIcon) return nullptr;

   Image->SetBrushFromTexture(ItemIcon, true);
   Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
   return Image;
}

void UInventoryWidget::ItemTooltipShow(const FItemButtonData& ItemButtonData)
{
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ItemTooltipEnable"));
   int32 Index = ItemButtonData.Index;
   FVector2D ButtonPosition = ItemButtonData.ButtonPosition;

   UBorder* Border = Cast<UBorder>(GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!Border) return;
   Border->SetVisibility(ESlateVisibility::Hidden);

   UTextBlock* ItemName = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTooltipNameValue")));
   UTextBlock* ItemType = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTooltipTypeValue")));
   UTextBlock* ItemPrice = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTooltipPriceValue")));
   UTextBlock* ItemStack = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTooltipStackValue")));

   if (!(ItemName && ItemType && ItemPrice && ItemStack)) return;
   UItem* ItemData = Inventory->GetItem(Index);
   ItemName->SetText(FText::FromName(ItemData->GetItemName()));
   ItemType->SetText(FText::FromName(ItemData->GetItemType()));
   ItemPrice->SetText(FText::FromString(FString::FromInt(ItemData->GetItemPrice())));
   if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(ItemData))
   {
      ItemStack->SetText(FText::FromString(
         FString::Printf(TEXT("%d / %d"), MaterialItem->GetItemCurrentStack(), MaterialItem->GetItemMaxStack())
      ));
   }
   else
   {
      ItemStack->SetText(FText::FromString(TEXT("1 / 1")));
   }
   
   UGridPanel* ItemTooltipGrid = Cast<UGridPanel>(GetWidgetFromName(TEXT("ItemTooltipGrid")));
   ItemTooltipGrid->ForceLayoutPrepass();
   FVector2D NewBorderSize = Cast<UWidget>(ItemTooltipGrid)->GetDesiredSize();
   NewBorderSize.X += 5;

   UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Border->Slot);
   if (!CanvasPanelSlot) return;
   CanvasPanelSlot->SetSize(NewBorderSize);

   FVector2D ItemTooltipPos = { ButtonPosition.X , ButtonPosition.Y - CanvasPanelSlot->GetSize().Y - 10 };

   UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasPanel")));
   FGeometry WidgetGeometry = Cast<UWidget>(CanvasPanel)->GetCachedGeometry();
   FVector2D CanvasSize = WidgetGeometry.GetLocalSize();

   if (ItemTooltipPos.X + NewBorderSize.X >= CanvasSize.X)
   {
      ItemTooltipPos.X -= ItemTooltipPos.X + NewBorderSize.X - CanvasSize.X;
   }

   CanvasPanelSlot->SetPosition(ItemTooltipPos);
   
   Border->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UInventoryWidget::ItemTooltipHide()
{
   UBorder* ItempTooltipBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!ItempTooltipBorder) return;

   ItempTooltipBorder->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::ItemContextMenuShow(const FItemButtonData& ItemButtonData)
{
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("ItemContextMenuShow"));
   int32 Index = ItemButtonData.Index;
   FVector2D ButtonPosition = ItemButtonData.ButtonPosition;

   UBorder* Border = Cast<UBorder>(GetWidgetFromName(TEXT("ItemContextMenuBorder")));
   if (!Border) return;
   Border->SetVisibility(ESlateVisibility::Hidden);
   Border->ForceLayoutPrepass();

   UBorderSlot* BolderSlot = Cast<UBorderSlot>(InventoryGrid->Slot);
   if (!BolderSlot) return;

   FVector2D BorderSize = Border->GetCachedGeometry().GetLocalSize();
   FVector2D MenuPos = { ButtonPosition.X, ButtonPosition.Y };
   MenuPos.Y += BolderSlot->GetPadding().Top - 5;
   MenuPos.X += BolderSlot->GetPadding().Left;

   UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Border->Slot);
   if (!CanvasPanelSlot) return;
   CanvasPanelSlot->SetAlignment(FVector2D(1.0f, 0.0f));
   CanvasPanelSlot->SetPosition(MenuPos);

   UseItemButtonWidget->SetButtonData(ItemButtonData);
   DestroyItemButtonWidget->SetButtonData(ItemButtonData);

   Border->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryWidget::ItemContextMenuHide()
{
   UBorder* ItemContextMenuBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ItemContextMenuBorder")));
   if (!ItemContextMenuBorder) return;

   ItemContextMenuBorder->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::ItemDestory(const FItemButtonData& ItemButtonData)
{
   Inventory->RemoveItemIndex(ItemButtonData.Index);
   ItemContextMenuHide();
}

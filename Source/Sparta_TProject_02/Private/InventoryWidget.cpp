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
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "ItemButtonWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "MyGameInstance.h"

void UInventoryWidget::NativeConstruct()
{
   Super::NativeConstruct();
   PlayerContlloer = GetOwningPlayer();
   
   InventoryGrid = Cast<UGridPanel>(GetWidgetFromName("GridPanel"));
   GridMaxColumn = 4;

   ItemContextMenuData = { 0, FVector2D::ZeroVector };
}
void UInventoryWidget::SetupWidget()
{
   UMyGameInstance* GameInstance = Cast<UMyGameInstance>(PlayerContlloer->GetGameInstance());
   if (GameInstance)
   {
      Inventory = GameInstance->Inventory;
   }
   else
   {
      GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("InventorySet Fail"));
   }

   UItemButtonWidget* UseItemButtonWidget = CreateWidget<UItemButtonWidget>(PlayerContlloer, UItemButtonWidget::StaticClass());
   UButton* UseButton = Cast<UButton>(GetWidgetFromName("UseButton"));
   if (!UseButton) return;
   UseItemButtonWidget->SetButton(UseButton);   
   UseItemButtonWidget->SetupClickBinding();
   //UseItemButtonWidget->OnClicked.AddDynamic(this, &UInventoryWidget::ItemTooltipShow);
   UseItemButtonWidget->SetButtonData(ItemContextMenuData);

   UItemButtonWidget* DestroyItemButtonWidget = CreateWidget<UItemButtonWidget>(PlayerContlloer, UItemButtonWidget::StaticClass());
   UButton* DestoryButton = Cast<UButton>(GetWidgetFromName("DestroyButton"));
   if (!DestoryButton) return;

   DestroyItemButtonWidget->SetButton(DestoryButton);
   DestroyItemButtonWidget->SetupClickBinding();
   DestroyItemButtonWidget->OnClicked.AddDynamic(this, &UInventoryWidget::ItemDestory);
   DestroyItemButtonWidget->SetButtonData(ItemContextMenuData);
}
bool UInventoryWidget::RefreshWidget()
{
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("RefreshWidget"));
   for (int32 i = InventoryGrid->GetChildrenCount(); i > 0; --i)
   {
      InventoryGrid->RemoveChildAt(0);
   }

   int32 ItemSize = Inventory->GetCurrentSize();
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, FString::Printf(TEXT("ItemSize: %d"), ItemSize));
   for (int32 i = 0; i < ItemSize; ++i)
   {
      if (!AddItemToGrid(Inventory->GetItem(i), i))
      {
         GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("AddItemToGrid Error"));
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

   UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Cast<UWidget>(Border));
   if (!CanvasPanelSlot) return false;

   FItemButtonData ButtonData;
   ButtonData.Index = Index;
   ButtonData.ButtonPosition = CanvasPanelSlot->GetPosition();
   ButtonData.ButtonPosition.X += (Column * ItemOverlaySize.X) + ((Column + 1) * ItemMargin.Left);
   ButtonData.ButtonPosition.Y += (Row * ItemOverlaySize.Y) + ((Row + 1) * ItemMargin.Top);

   UOverlay* ItemOverlay = CreateItemOverlay(Item, ItemOverlaySize, ButtonData);
   UGridSlot* GridSlot = InventoryGrid->AddChildToGrid(Cast<UWidget>(ItemOverlay));

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

   UItemButtonWidget* ItemButtonWidget = CreateItemButton(Item, ItemOverlaySize, ButtonData);
   if (!ItemButtonWidget) return nullptr;
   ItemOverlay->AddChild(Cast<UWidget>(ItemButtonWidget->GetButton()));

   if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(Item))
   {
      UTextBlock* ItemStackTextBlock = CreateItemStackTextBlock(Item);
      if (!ItemStackTextBlock) return nullptr;
      ItemOverlay->AddChild(Cast<UWidget>(ItemStackTextBlock));

      UOverlaySlot* OverlaySlot = ItemOverlay->AddChildToOverlay(ItemStackTextBlock);
      OverlaySlot->SetPadding(FMargin(0.0f, 0.0f, 5.0f, 5.0f));
      OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
      OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
   }
   return ItemOverlay;
}

UItemButtonWidget* UInventoryWidget::CreateItemButton(UItem* Item, const FVector2D ItemButtonSize, FItemButtonData ButtonData)
{
   UItemButtonWidget* ItemButtonWidget = CreateWidget<UItemButtonWidget>(PlayerContlloer, UItemButtonWidget::StaticClass());
   ItemButtonWidget->SetButton(NewObject<UButton>(PlayerContlloer));
   ItemButtonWidget->SetupHoverBinding();
   ItemButtonWidget->SetupClickBinding();

   FDeprecateSlateVector2D ButtonSize = FDeprecateSlateVector2D(ItemButtonSize.X, ItemButtonSize.Y);
   FButtonStyle ButtonStyle = ItemButtonWidget->GetButton()->GetStyle();

   FSlateBrush NormalSB = ButtonStyle.Normal;
   NormalSB.SetImageSize(ButtonSize);
   ButtonStyle.SetNormal(NormalSB);

   FSlateBrush HoveredSB = ButtonStyle.Hovered;
   HoveredSB.SetImageSize(ButtonSize);
   ButtonStyle.SetHovered(HoveredSB);

   FSlateBrush PressedSB = ButtonStyle.Pressed;
   PressedSB.SetImageSize(ButtonSize);
   ButtonStyle.SetPressed(PressedSB);

   ItemButtonWidget->GetButton()->SetStyle(ButtonStyle);
   ItemButtonWidget->OnHovered.AddDynamic(this, &UInventoryWidget::ItemTooltipShow);
   ItemButtonWidget->GetButton()->OnUnhovered.AddDynamic(this, &UInventoryWidget::ItemTooltipHide);
   ItemButtonWidget->OnClicked.AddDynamic(this, &UInventoryWidget::ItemContextMenuShow);
   ItemButtonWidget->SetButtonData(ButtonData);

   return ItemButtonWidget;
}

UTextBlock* UInventoryWidget::CreateItemStackTextBlock(UItem* Item)
{
   UTextBlock* TextBlock = NewObject<UTextBlock>(PlayerContlloer);
   if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(Item))
   {
      //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("MaterialItem"));
      TextBlock->SetText(FText::FromString(FString::FromInt(MaterialItem->GetItemCurrentStack())));
   }
   else
   {
      TextBlock->SetText(FText::FromString(TEXT("0")));
   }
   return TextBlock;
}

void UInventoryWidget::ItemTooltipShow(const FItemButtonData& ItemButtonData)
{
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ItemTooltipEnable"));
   int32 Index = ItemButtonData.Index;
   FVector2D ButtonPosition = ItemButtonData.ButtonPosition;

   UBorder* Border = Cast<UBorder>(GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!Border) return;
   Cast<UWidget>(Border)->SetVisibility(ESlateVisibility::Hidden);

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
   FVector2D NewBorderSize = Cast<UWidget>(ItemTooltipGrid)->GetDesiredSize();
   NewBorderSize.X += 5;
   ForceLayoutPrepass();

   UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Cast<UWidget>(Border)->Slot);
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
   
   Cast<UWidget>(Border)->SetVisibility(ESlateVisibility::HitTestInvisible);
   //ForceLayoutPrepass();
}

void UInventoryWidget::ItemTooltipHide()
{
   UBorder* ItempTooltipBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!ItempTooltipBorder) return;

   Cast<UWidget>(ItempTooltipBorder)->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::ItemContextMenuShow(const FItemButtonData& ItemButtonData)
{
   GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("ItemContextMenuShow"));
   int32 Index = ItemButtonData.Index;
   FVector2D ButtonPosition = ItemButtonData.ButtonPosition;

   UBorder* Border = Cast<UBorder>(GetWidgetFromName(TEXT("ItemContextMenuBorder")));
   if (!Border) return;
   Cast<UWidget>(Border)->SetVisibility(ESlateVisibility::Hidden);

   FGeometry WidgetGeometry = Cast<UWidget>(Border)->GetCachedGeometry();
   FVector2D BorderSize = WidgetGeometry.GetLocalSize();
   FVector2D MenuPos = { ButtonPosition.X, ButtonPosition.Y };

   MenuPos.X -= BorderSize.X;
   MenuPos.Y -= 5;

   UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Cast<UWidget>(Border)->Slot);
   if (!CanvasPanelSlot) return;
   CanvasPanelSlot->SetPosition(MenuPos);

   Cast<UWidget>(Border)->SetVisibility(ESlateVisibility::Visible);
   ForceLayoutPrepass();
   ItemContextMenuData = ItemButtonData;
}

void UInventoryWidget::ItemContextMenuHide()
{
   UBorder* ItemContextMenuBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ItemContextMenuBorder")));
   if (!ItemContextMenuBorder) return;

   Cast<UWidget>(ItemContextMenuBorder)->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::ItemDestory(const FItemButtonData& ItemButtonData)
{
   GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("DestoryIndex = " + ItemButtonData.Index));
}

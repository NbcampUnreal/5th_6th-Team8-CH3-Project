#include "EquipmentWidget.h"
#include "Item.h"
#include "EquipmentItem.h"
#include "Gem.h"
#include "AttackGem.h"
#include "DefenseGem.h"
#include "SpeedGem.h"
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
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "ItemButtonWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "MyGameInstance.h"

UEquipmentWidget::UEquipmentWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
   Inventory = nullptr;
   GemSlots = nullptr;
   GemSlotsGrid = nullptr;
   SelectGrid = nullptr;
   UnEquipBox = nullptr;
   UnEquipButton = nullptr;
   GridMaxColumn = 6;
   PlayerContlloer = nullptr;
   DynamicButtonClass = nullptr;
}

void UEquipmentWidget::NativeConstruct()
{
   Super::NativeConstruct();
   PlayerContlloer = GetOwningPlayer();
   GemSlotsGrid = Cast<UGridPanel>(GetWidgetFromName(TEXT("GemSlotsGrid")));
   SelectGrid = Cast<UGridPanel>(GetWidgetFromName(TEXT("EquipmentSelectGrid")));
   UnEquipBox = Cast<USizeBox>(GetWidgetFromName(TEXT("UnEquipBox")));
   UnEquipButton = Cast<UItemButtonWidget>(GetWidgetFromName(TEXT("UnEquipSelectButton")));
   UBorder* Border = Cast<UBorder>(GetWidgetFromName(TEXT("ItemContextMenuBorder")));
   if (!Border) return;
}

void UEquipmentWidget::SetupWidget()
{
   UMyGameInstance* GameInstance = Cast<UMyGameInstance>(PlayerContlloer->GetGameInstance());
   if (GameInstance)
   {
      Inventory = GameInstance->Inventory;
      GemSlots = GameInstance->GemSlots;
      GemSlots->SetMaxSize(GridMaxColumn);
      RefreshWidget();
   }
   if (!UnEquipButton) return;
   UnEquipButton->OnClicked.AddDynamic(this, &UEquipmentWidget::UnEquip);
   UnEquipButton->SetupClickBinding();
}
bool UEquipmentWidget::RefreshWidget()
{
   EquipmentSelectHide();
   ItemTooltipHide();
   for (int32 i = GemSlotsGrid->GetChildrenCount(); i > 0; --i)
   {
      GemSlotsGrid->RemoveChildAt(0);
   }

   for (int32 i = SelectGrid->GetChildrenCount(); i > 1; --i)
   {
      SelectGrid->RemoveChildAt(1);
   }

   int32 GmeSize = GemSlots->GetCurrentSize();
   int32 GmeMAxSize = GemSlots->GetMaxSize();
   for (int32 i = 0; i < GmeSize; ++i)
   {
      if (!AddGemToGrid(GemSlots->GetItem(i), i))
      {
         return false;
      }
   }

   for (int32 i = GmeSize; i < GmeMAxSize; ++i)
   {
      if (!AddGemToGrid(nullptr, i))
      {
         return false;
      }
   }

   return true;
}

bool UEquipmentWidget::AddGemToGrid(UItem* Item, int32 Index)
{
   int32 Row = 0;
   int32 Column = Index % GridMaxColumn;
   FVector2D ItemOverlaySize = FVector2D(100.0f, 100.0f);
   FMargin ItemMargin = FMargin(5.0f, 5.0f, 0.0f, 0.0f);

   UBorder* Border = Cast<UBorder>(Cast<UWidget>(GemSlotsGrid)->GetParent());
   if (!Border) return false;

   UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Cast<UWidget>(Border));
   if (!CanvasPanelSlot) return false;

   FItemButtonData ButtonData;
   ButtonData.Index = Index;
   ButtonData.ButtonPosition = CanvasPanelSlot->GetPosition();
   ButtonData.ButtonPosition.X += (Column * ItemOverlaySize.X) + ((Column + 1) * ItemMargin.Left);
   ButtonData.ButtonPosition.Y += (Row * ItemOverlaySize.Y) + ((Row + 1) * ItemMargin.Top);

   UOverlay* ItemOverlay = CreateItemOverlay(Item, ItemOverlaySize, ButtonData);
   UGridSlot* GridSlot = GemSlotsGrid->AddChildToGrid(Cast<UWidget>(ItemOverlay));

   GridSlot->SetPadding(ItemMargin);
   GridSlot->SetRow(Row);
   GridSlot->SetColumn(Column);
   return true;
}

UOverlay* UEquipmentWidget::CreateItemOverlay(UItem* Item, const FVector2D& ItemOverlaySize, FItemButtonData ButtonData)
{
   UOverlay* ItemOverlay = NewObject<UOverlay>(PlayerContlloer);
   if (!ItemOverlay) return nullptr;

   UItemButtonWidget* ItemButtonWidget = CreateItemButton(Item, ItemOverlaySize, ButtonData);
   if (!ItemButtonWidget) return nullptr;
   if (Item)
   {
      ItemButtonWidget->OnHovered.AddDynamic(this, &UEquipmentWidget::ItemTooltipShow);
      ItemButtonWidget->GetButton()->OnUnhovered.AddDynamic(this, &UEquipmentWidget::ItemTooltipHide);
   }
   ItemButtonWidget->OnClicked.AddDynamic(this, &UEquipmentWidget::GemSelectShow);
   ButtonData.Index += 1;
   ButtonData.Index = -ButtonData.Index;
   ItemButtonWidget->SetButtonData(ButtonData);

   ItemOverlay->AddChild(Cast<UWidget>(ItemButtonWidget));

   if (Item)
   {
      UImage* Image = CreateItemImage(Item, ItemOverlay);
      if (Image)
      {
         UOverlaySlot* OverlaySlot = ItemOverlay->AddChildToOverlay(Image);
         OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
         OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
      }
   }
   return ItemOverlay;
}

UItemButtonWidget* UEquipmentWidget::CreateItemButton(UItem* Item, const FVector2D ItemButtonSize, FItemButtonData ButtonData)
{
   if (!DynamicButtonClass) return nullptr;
   UItemButtonWidget* ItemButtonWidget = CreateWidget<UItemButtonWidget>(this, DynamicButtonClass);
   UButton* InnerButton = ItemButtonWidget->GetButton();
   ItemButtonWidget->SetupHoverBinding();
   ItemButtonWidget->SetupClickBinding();

   FDeprecateSlateVector2D ButtonSize = FDeprecateSlateVector2D(ItemButtonSize.X, ItemButtonSize.Y);
   FButtonStyle ButtonStyle = InnerButton->GetStyle();

   FSlateBrush NormalSB = ButtonStyle.Normal;
   NormalSB.SetImageSize(ButtonSize);
   if (!Item)
   {
      NormalSB.TintColor = FSlateColor(FLinearColor::Black);
   }
   else
   {
      NormalSB.TintColor = FSlateColor(FLinearColor::White);
   }
   ButtonStyle.SetNormal(NormalSB);

   FSlateBrush HoveredSB = ButtonStyle.Hovered;
   HoveredSB.SetImageSize(ButtonSize);
   ButtonStyle.SetHovered(HoveredSB);

   FSlateBrush PressedSB = ButtonStyle.Pressed;
   PressedSB.SetImageSize(ButtonSize);
   ButtonStyle.SetPressed(PressedSB);

   InnerButton->SetStyle(ButtonStyle);
   return ItemButtonWidget;
}

UImage* UEquipmentWidget::CreateItemImage(UItem* Item, UOverlay* Overlay)
{
   UImage* Image = NewObject<UImage>(Overlay);
   UTexture2D* ItemIcon = Item->GetItemIcon();
   if (!ItemIcon) return nullptr;

   Image->SetBrushFromTexture(ItemIcon, true);
   Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
   return Image;
}

void UEquipmentWidget::ItemTooltipShow(const FItemButtonData& ItemButtonData)
{
   int32 Index = ItemButtonData.Index;
   FVector2D ButtonPosition = ItemButtonData.ButtonPosition;

   UBorder* Border = Cast<UBorder>(GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!Border) return;
   Cast<UWidget>(Border)->SetVisibility(ESlateVisibility::Hidden);

   UTextBlock* ItemName = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTooltipNameValue")));
   UTextBlock* ItemType = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTooltipTypeValue")));
   UTextBlock* ItemPrice = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTooltipPriceValue")));
   UTextBlock* ItemStat = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTooltipStatValue")));
   UTextBlock* ItemStatName = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemTooltipStat")));

   if (!(ItemName && ItemType && ItemPrice && ItemStat)) return;
   UItem* ItemData;
   if (Index < 0)
   {
      Index += 1;
      Index = -Index;
      ItemData = GemSlots->GetItem(Index);
   }
   else 
   {
      ItemData = Inventory->GetItem(Index); 
   }
   ItemName->SetText(FText::FromName(ItemData->GetItemName()));
   ItemType->SetText(FText::FromName(ItemData->GetItemType()));
   ItemPrice->SetText(FText::FromString(FString::FromInt(ItemData->GetItemPrice())));

   if (UEquipmentItem* EquipmentItem = Cast<UEquipmentItem>(ItemData))
   {
      if (UGem* Gem = Cast<UGem>(EquipmentItem))
      {
         if (UAttackGem* ATKGem = Cast<UAttackGem>(Gem))
         {
            ItemStatName->SetText(FText::FromString("Attack : "));
            ItemStat->SetText(FText::FromString(FString::FromInt(ATKGem->GetAttackValue())));
         }
         else if (UDefenseGem* DFSGem = Cast<UDefenseGem>(Gem))
         {
            ItemStatName->SetText(FText::FromString("Defense : "));
            ItemStat->SetText(FText::FromString(FString::FromInt(DFSGem->GetDefenseValue())));
         }
         else if (USpeedGem* SPDGem = Cast<USpeedGem>(Gem))
         {
            ItemStatName->SetText(FText::FromString("Speed : "));
            ItemStat->SetText(FText::FromString(FString::FromInt(SPDGem->GetSpeedValue())));
         }
      }
   }

   UGridPanel* ItemTooltipGrid = Cast<UGridPanel>(GetWidgetFromName(TEXT("ItemTooltipGrid")));
   ItemTooltipGrid->ForceLayoutPrepass();
   FVector2D NewBorderSize = Cast<UWidget>(ItemTooltipGrid)->GetDesiredSize();
   NewBorderSize.X += 5;

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

   Cast<UWidget>(Border)->SetVisibility(ESlateVisibility::Visible);
}

void UEquipmentWidget::ItemTooltipHide()
{
   UBorder* ItempTooltipBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!ItempTooltipBorder) return;

   Cast<UWidget>(ItempTooltipBorder)->SetVisibility(ESlateVisibility::Collapsed);
}

bool UEquipmentWidget::AddGemToSelectGrid(UItem* Item, int32 Index, int32 Column)
{
   int32 Row = 0;
   FVector2D ItemOverlaySize = FVector2D(50.0f, 50.0f);
   FMargin ItemMargin = FMargin(5.0f);

   UBorder* Border = Cast<UBorder>(Cast<UWidget>(SelectGrid)->GetParent());
   if (!Border) return false;

   UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Cast<UWidget>(Border));
   if (!CanvasPanelSlot) return false;

   FItemButtonData ButtonData;
   ButtonData.Index = Index;
   ButtonData.ButtonPosition = CanvasPanelSlot->GetPosition();
   ButtonData.ButtonPosition.X += (Column * ItemOverlaySize.X) + ((Column + 1) * ItemMargin.Left);
   ButtonData.ButtonPosition.Y += (Row * ItemOverlaySize.Y) + ((Row + 1) * ItemMargin.Top);

   USizeBox* SizeBox = NewObject<USizeBox>(PlayerContlloer);
   if (!SizeBox) return false;
   SizeBox->SetWidthOverride(ItemOverlaySize.X);
   SizeBox->SetHeightOverride(ItemOverlaySize.Y);

   UOverlay* ItemOverlay = CreateSelectItemOverlay(Item, ItemOverlaySize, ButtonData);
   if (!ItemOverlay) return false;
   SizeBox->SetContent(ItemOverlay);

   UGridSlot* GridSlot = SelectGrid->AddChildToGrid(Cast<UWidget>(SizeBox));

   GridSlot->SetPadding(ItemMargin);
   GridSlot->SetRow(Row);
   GridSlot->SetColumn(Column);
   return true;
}

UOverlay* UEquipmentWidget::CreateSelectItemOverlay(UItem* Item, const FVector2D& ItemOverlaySize, FItemButtonData ButtonData)
{
   UOverlay* ItemOverlay = NewObject<UOverlay>(PlayerContlloer);
   if (!ItemOverlay) return nullptr;

   UItemButtonWidget* ItemButtonWidget = CreateItemButton(Item, ItemOverlaySize, ButtonData);
   if (!ItemButtonWidget) return nullptr;

   ItemButtonWidget->OnHovered.AddDynamic(this, &UEquipmentWidget::ItemTooltipShow);
   ItemButtonWidget->GetButton()->OnUnhovered.AddDynamic(this, &UEquipmentWidget::ItemTooltipHide);
   ItemButtonWidget->OnClicked.AddDynamic(this, &UEquipmentWidget::Equip);
   ItemButtonWidget->SetButtonData(ButtonData);

   ItemOverlay->AddChild(Cast<UWidget>(ItemButtonWidget));

   UImage* Image = CreateItemImage(Item, ItemOverlay);
   if (Image)
   {
      UOverlaySlot* OverlaySlot = ItemOverlay->AddChildToOverlay(Image);
      OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
      OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
   }

   return ItemOverlay;
}

void UEquipmentWidget::GemSelectShow(const FItemButtonData& ItemButtonData)
{
   for (int32 i = SelectGrid->GetChildrenCount(); i > 1; --i)
   {
      SelectGrid->RemoveChildAt(1);
   }

   UBorder* Border = Cast<UBorder>(GetWidgetFromName(TEXT("EquipmentSelectBorder")));
   if (!Border) return;
   Cast<UWidget>(Border)->SetVisibility(ESlateVisibility::Collapsed);

   int32 Index = ItemButtonData.Index;
   FVector2D ButtonPosition = ItemButtonData.ButtonPosition;
   int32 Column = 0;

   if (Index < 0)
   {
      Index += 1;
      Index = -Index;
   }
   if (GemSlots->IsValidIdx(Index)) // 선택한 GemtSlot이 filled
   {

      UnEquipBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
      UnEquipButton->SetButtonData({ Index , ButtonPosition });
      ++Column;
   }
   else // 선택한 GemtSlot이 empty
   {
      UnEquipBox->SetVisibility(ESlateVisibility::Collapsed);
      UnEquipButton->SetButtonData(ItemButtonData);
   }
   TArray<int32> GemIndexs = Inventory->FindItemType("Gem");
   int32 InventorySize = Inventory->GetCurrentSize();

   for (int32 GemIndex : GemIndexs)
   {
      UItem* GemItem = Inventory->GetItem(GemIndex);
      AddGemToSelectGrid(GemItem, GemIndex, Column);
      ++Column;
   }
   if (!GemSlots->IsValidIdx(Index) && GemIndexs.IsEmpty()) return;

   Cast<UWidget>(Border)->SetVisibility(ESlateVisibility::Visible);
}

void UEquipmentWidget::EquipmentSelectHide()
{
   UBorder* EquipmentSelectBorder = Cast<UBorder>(GetWidgetFromName(TEXT("EquipmentSelectBorder")));
   if (!EquipmentSelectBorder) return;

   Cast<UWidget>(EquipmentSelectBorder)->SetVisibility(ESlateVisibility::Collapsed);
}

void UEquipmentWidget::Equip(const FItemButtonData& ItemButtonData)
{
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Equip"));
   int32 Index = ItemButtonData.Index;

   UItem* Item = Inventory->GetItem(Index);
   Inventory->RemoveItemIndex(Index);
   GemSlots->AddItem(Item);
   EquipmentSelectHide();
   RefreshWidget();
}

void UEquipmentWidget::UnEquip(const FItemButtonData& ItemButtonData)
{
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("UnEquip"));
   int32 Index = ItemButtonData.Index;

   UItem* Item = GemSlots->GetItem(Index);
   GemSlots->RemoveItemIndex(Index);
   Inventory->AddItem(Item);
   EquipmentSelectHide();
   RefreshWidget();
}


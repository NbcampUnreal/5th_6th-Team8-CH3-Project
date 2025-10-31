#include "MyGameInstance.h"
#include "Item.h"
#include "MaterialItem.h"
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

UMyGameInstance::UMyGameInstance()
{
   Inventory = nullptr;
   InventoryWidgetClass = nullptr;
   InventoryWidgetInstance = nullptr;
   GridPanel = nullptr;
	static ConstructorHelpers::FClassFinder<UUserWidget> InvenHUDFInder(
		TEXT("/Game/Blueprints/HUD_Inventory.HUD_Inventory_C"));
   if (InvenHUDFInder.Succeeded())
   {
      InventoryWidgetClass = InvenHUDFInder.Class;
   }
   
}

void UMyGameInstance::Init()
{
   Super::Init();
   Inventory = NewObject<UInventory>(this);
   //Inventory = UInventory::GetInstance();
}

void UMyGameInstance::WidgetInstall()
{
	//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	InventoryWidgetInstance = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
	InventoryWidgetInstance->AddToViewport();

   GridPanel = Cast<UGridPanel>(InventoryWidgetInstance->GetWidgetFromName(TEXT("GridPanel")));
   ItemTooltipDisable();
}

void UMyGameInstance::AddItemButton(UItem* Item) {
   int32 GridSize = 4;
   int32 ItemSize = Inventory->GetCurrentSize();
   int32 ItemMaxSize = Inventory->GetMaxSize();

   if (!GridPanel) return;
   UItemButtonWidget* ItemButtonWidget = CreateWidget<UItemButtonWidget>(this, UItemButtonWidget::StaticClass());
   ItemButtonWidget->SetButton(NewObject<UButton>(this));
   ItemButtonWidget->ButtonBinding();
   FItemButtonData ButtonData;
   ButtonData.Index = ItemSize;
   ButtonData.ButtonPosition = FVector2D::ZeroVector;

   FDeprecateSlateVector2D ButtonSize(100.0f, 100.0f);
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
   ItemButtonWidget->OnHovered.AddDynamic(this, &UMyGameInstance::ItemTooltipEnable);
   ItemButtonWidget->GetButton()->OnUnhovered.AddDynamic(this, &UMyGameInstance::ItemTooltipDisable);
   
   UTextBlock* ItemOwned = nullptr;
   UOverlay* ButtonOverlay = NewObject<UOverlay>(this);
   if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(Item))
   {
      ItemOwned = NewObject<UTextBlock>(this);
      ItemOwned->SetText(FText::FromString(FString::FromInt(MaterialItem->GetCurrentStack())));
      UOverlaySlot* OverlaySlot = ButtonOverlay->AddChildToOverlay(ItemOwned);

      OverlaySlot->SetPadding(FMargin(0.0f, 0.0f, 5.0f, 5.0f));
      OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
      OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
   }

   ItemButtonWidget->GetButton()->SetContent(Cast<UWidget>(ButtonOverlay));

   // 버튼 위치를 옮겨서 그리드(오버레이(버튼,이미지,텍스트) ) 구조로 만들자.

   UGridSlot* GridSlot = GridPanel->AddChildToGrid(Cast<UWidget>(ItemButtonWidget->GetButton()));
   GridSlot->SetPadding(FMargin(5.0f, 5.0f, 0.0f, 0.0f));
   GridSlot->SetRow(ItemSize / GridSize);
   GridSlot->SetColumn(ItemSize % GridSize);

   UBorder* Border = Cast<UBorder>(GridPanel->GetParent());
   if (!Border) return;

   UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Cast<UWidget>(Border));
   if (!CanvasPanelSlot) return;

   ButtonData.ButtonPosition = CanvasPanelSlot->GetPosition();
   ButtonData.ButtonPosition.X += (GridSlot->GetColumn() * ButtonSize.X) + ((GridSlot->GetColumn() + 1) * GridSlot->GetPadding().Left);
   ButtonData.ButtonPosition.Y += (GridSlot->GetRow() * ButtonSize.Y) + ((GridSlot->GetRow() + 1) * GridSlot->GetPadding().Top);
   
   GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
      FString::Printf(TEXT("Postion X: %f Y: %f"), ButtonData.ButtonPosition.X, ButtonData.ButtonPosition.Y));
   //ButtonData.ButtonPosition = FVector2D(500.0f, 500.0f);
   ItemButtonWidget->SetButtonData(ButtonData);
}

void UMyGameInstance::ItemTooltipEnable(const FItemButtonData& ItemButtonData)
{
   GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ItemTooltipEnable"));
   int32 Index = ItemButtonData.Index;
   FVector2D ButtonPosition = ItemButtonData.ButtonPosition;
   
   UBorder* Border = Cast<UBorder>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!Border) return;

   UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Border->Slot);
   if (!CanvasPanelSlot) return;

   UTextBlock* ItemName = Cast<UTextBlock>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipNameValue")));
   UTextBlock* ItemType = Cast<UTextBlock>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipTypeValue")));
   UTextBlock* ItemPrice = Cast<UTextBlock>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipPriceValue")));
   UTextBlock* ItemStack = Cast<UTextBlock>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipStackValue")));

   if (!(ItemName && ItemType && ItemPrice && ItemStack)) return;
   UItem* ItemData = Inventory->GetItem(Index);
   ItemName->SetText(FText::FromName(ItemData->GetItemName()));
   ItemType->SetText(FText::FromName(ItemData->GetItemType()));
   ItemPrice->SetText(FText::FromString(FString::FromInt(ItemData->GetItemPrice())));
   if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(ItemData))
   {
      ItemStack->SetText(FText::FromString(
         FString::Printf(TEXT("%d / %d"), MaterialItem->GetCurrentStack(), MaterialItem->GetItemMaxStack())
      ));
   }
   else
   {
      ItemStack->SetText(FText::FromString(TEXT("1 / 1")));
   }
   UGridPanel* ItemTooltipGrid = Cast<UGridPanel>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipGrid")));
   //ItemTooltipGrid->InvalidateLayoutAndDesiredSize();
   ItemTooltipGrid->ForceLayoutPrepass();
   FVector2D NewBorderSize = ItemTooltipGrid->GetDesiredSize();
   NewBorderSize.X += 5;
   CanvasPanelSlot->SetSize(NewBorderSize);

   FVector2D ItemTooltipPos = { ButtonPosition.X , ButtonPosition.Y - CanvasPanelSlot->GetSize().Y - 10 };
   
   UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(InventoryWidgetInstance->GetWidgetFromName(TEXT("CanvasPanel")));
   FGeometry WidgetGeometry = CanvasPanel->GetCachedGeometry();
   FVector2D CanvasSize = WidgetGeometry.GetLocalSize();

   if (ItemTooltipPos.X + NewBorderSize.X >= CanvasSize.X)
   {
      ItemTooltipPos.X -= ItemTooltipPos.X + NewBorderSize.X - CanvasSize.X;
   }
   
   CanvasPanelSlot->SetPosition(ItemTooltipPos);

   Border->SetVisibility(ESlateVisibility::Visible);
}

void UMyGameInstance::ItemTooltipDisable()
{
   UBorder* ItempTooltipBorder = Cast<UBorder>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!ItempTooltipBorder) return;

   ItempTooltipBorder->SetVisibility(ESlateVisibility::Collapsed);
}

#include "MyGameInstance.h"
#include "Item.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateBrush.h"
#include "Layout/Margin.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

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
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	InventoryWidgetInstance = CreateWidget<UUserWidget>(PlayerController, InventoryWidgetClass);
	InventoryWidgetInstance->AddToViewport();

   GridPanel = Cast<UGridPanel>(InventoryWidgetInstance->GetWidgetFromName(TEXT("GridPanel")));
   ItemTooltipDisable();
}

void UMyGameInstance::AddItemButton(UItem* Item) {
   int32 GridSize = 4;
   int32 ItemSize = Inventory->GetCurrentSize();
   int32 ItemMaxSize = Inventory->GetMaxSize();

   if (!GridPanel) return;
   UButton* Button = NewObject<UButton>(this);

   FDeprecateSlateVector2D ButtonSize(100.0f, 100.0f);
   FButtonStyle ButtonStyle = Button->GetStyle();

   FSlateBrush NormalSB = ButtonStyle.Normal;
   NormalSB.SetImageSize(ButtonSize);
   ButtonStyle.SetNormal(NormalSB);

   FSlateBrush HoveredSB = ButtonStyle.Hovered;
   HoveredSB.SetImageSize(ButtonSize);
   ButtonStyle.SetHovered(HoveredSB);

   FSlateBrush PressedSB = ButtonStyle.Pressed;
   PressedSB.SetImageSize(ButtonSize);
   ButtonStyle.SetPressed(PressedSB);

   Button->SetStyle(ButtonStyle);
   //Button->OnHovered.AddDynamic(this, &UMyGameInstance::ItemTooltipEnable);
   //Button->OnHovered(FSi)
   //Button->OnUnhovered.AddDynamic(this, &UMyGameInstance::ItemTooltipDisable);

   UGridSlot* GridSlot = GridPanel->AddChildToGrid(Button);
   GridSlot->SetPadding(FMargin(5.0f, 5.0f, 0.0f, 0.0f));
   GridSlot->SetRow(ItemSize / GridSize);
   GridSlot->SetColumn(ItemSize % GridSize);
}

void UMyGameInstance::ItemTooltipEnable(int32 Index, FVector2D ButtonPosition)
{
   UBorder* ItempTooltipBorder = Cast<UBorder>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!ItempTooltipBorder) return;

   ItempTooltipBorder->SetVisibility(ESlateVisibility::Hidden);

   UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(ItempTooltipBorder->Slot);
   if (!CanvasPanelSlot) return;

   CanvasPanelSlot->SetPosition({ ButtonPosition.X , ButtonPosition.Y });

   UTextBlock* ItemTooltipName = Cast<UTextBlock>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipName")));
   UTextBlock* ItemTooltipType = Cast<UTextBlock>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipType")));
   UTextBlock* ItemTooltipPrice = Cast<UTextBlock>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipPrice")));
   UTextBlock* ItemTooltipStack = Cast<UTextBlock>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipStack")));

   if (!(ItemTooltipName && ItemTooltipType && ItemTooltipPrice && ItemTooltipStack)) return;
   UItem* ItemData = Inventory->GetItem(Index);
   ItemTooltipName->SetText(FText::FromName(ItemData->GetItemName()));
}

void UMyGameInstance::ItemTooltipDisable()
{
   UBorder* ItempTooltipBorder = Cast<UBorder>(InventoryWidgetInstance->GetWidgetFromName(TEXT("ItemTooltipBorder")));
   if (!ItempTooltipBorder) return;

   ItempTooltipBorder->SetVisibility(ESlateVisibility::Collapsed);
}

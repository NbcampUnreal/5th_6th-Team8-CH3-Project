#include "MyGameInstance.h"
#include "InventoryWidget.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerCharacterController.h"

UMyGameInstance::UMyGameInstance()
{
   Inventory = nullptr;
   InventoryWidgetClass = nullptr;
   InventoryWidgetInstance = nullptr;
	static ConstructorHelpers::FClassFinder<UInventoryWidget> InvenHUDFInder(
		TEXT("/Game/Blueprints/WBP_InventoryWidget.WBP_InventoryWidget_C"));
   if (InvenHUDFInder.Succeeded())
   {
      InventoryWidgetClass = InvenHUDFInder.Class;
   }
}

UInventoryWidget* UMyGameInstance::GetInventoryWidget() const
{
   return InventoryWidgetInstance;
}

void UMyGameInstance::Init()
{
   Super::Init();
}

void UMyGameInstance::SetupInventoryWidget(APlayerCharacterController* PlayerContorller)
{
   if (!PlayerContorller) return;
   Inventory = NewObject<UInventory>(PlayerContorller);

   if (!InventoryWidgetClass) return;
   InventoryWidgetInstance = CreateWidget<UInventoryWidget>(PlayerContorller, InventoryWidgetClass);

   InventoryWidgetInstance->AddToViewport();
   InventoryWidgetInstance->SetupWidget();
   InventoryWidgetInstance->ItemTooltipHide();
   InventoryWidgetInstance->ItemContextMenuHide();
}
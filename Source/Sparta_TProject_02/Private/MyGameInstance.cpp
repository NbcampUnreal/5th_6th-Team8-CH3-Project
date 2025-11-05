#include "MyGameInstance.h"
#include "InventoryWidget.h"
#include "EquipmentWidget.h"
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

   GemSlots = nullptr;
   EquipmentWidgetClass = nullptr;
   EquipmentWidgetInstance = nullptr;
   static ConstructorHelpers::FClassFinder<UEquipmentWidget> EquipHUDFInder(
      TEXT("/Game/Blueprints/WBP_EquipmentWidget.WBP_EquipmentWidget_C"));
   if (EquipHUDFInder.Succeeded())
   {
      EquipmentWidgetClass = EquipHUDFInder.Class;
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

void UMyGameInstance::SetupEquipmentWidget(APlayerCharacterController* PlayerContorller)
{
   if (!PlayerContorller) return;
   GemSlots = NewObject<UInventory>(PlayerContorller);

   if (!EquipmentWidgetClass) return;
   EquipmentWidgetInstance = CreateWidget<UEquipmentWidget>(PlayerContorller, EquipmentWidgetClass);
   GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("EquipmentWidgetClass Success"));

   EquipmentWidgetInstance->AddToViewport();
   EquipmentWidgetInstance->SetupWidget();
   EquipmentWidgetInstance->ItemTooltipHide();
   EquipmentWidgetInstance->EquipmentSelectHide();
}
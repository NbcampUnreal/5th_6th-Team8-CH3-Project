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

   static ConstructorHelpers::FClassFinder<UInventory> InventoryClassFinder(
	   TEXT("/Game/Blueprints/BPC_Inventory.BPC_Inventory_C"));
   if (InventoryClassFinder.Succeeded())
   {
	   InventoryBlueprintClass = InventoryClassFinder.Class;
   }

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

	UClass* ClassToSpawn = InventoryBlueprintClass.Get() ? InventoryBlueprintClass.Get() : UInventory::StaticClass();
	Inventory = NewObject<UInventory>(PlayerContorller, ClassToSpawn);

	//Inventory = NewObject<UInventory>(PlayerContorller);

	if (!InventoryWidgetClass) return;
	InventoryWidgetInstance = CreateWidget<UInventoryWidget>(PlayerContorller, InventoryWidgetClass);

	InventoryWidgetInstance->AddToViewport();
	InventoryWidgetInstance->SetupWidget();

	InventoryWidgetInstance->SetVisibility(ESlateVisibility::Visible);

	InventoryWidgetInstance->ItemTooltipHide();
	InventoryWidgetInstance->ItemContextMenuHide();
}
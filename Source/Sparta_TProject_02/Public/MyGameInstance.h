#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Inventory.h"
#include "Components/Button.h"
#include "MyGameInstance.generated.h"

class UUserWidget;
class UGridPanel;

UCLASS()
class SPARTA_TPROJECT_02_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	UUserWidget* InventoryWidgetInstance;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	UGridPanel* GridPanel;

	virtual void Init() override;
	
public:
	UMyGameInstance();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventory* Inventory;
	
	void WidgetInstall();
	void AddItemButton(UItem* Item);

	UFUNCTION()
	void ItemTooltipEnable(int32 Index, FVector2D ButtonPosition);
	UFUNCTION()
	void ItemTooltipDisable();
};

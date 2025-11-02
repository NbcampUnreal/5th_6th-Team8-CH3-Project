#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemButtonWidget.h"
#include "InventoryWidget.generated.h"

class UItem;
class UInventory;
class UGridPanel;
class UOverlay;
class UItemButtonWidget;
class UTextBlock;
struct FItemButtonData;

UCLASS()
class SPARTA_TPROJECT_02_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventory* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UGridPanel* InventoryGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 GridMaxRows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 GridMaxColumn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	APlayerController* PlayerContlloer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	FItemButtonData ItemContextMenuData;

	virtual void NativeConstruct() override;
public:
	void SetupWidget();
	bool RefreshWidget();
	bool AddItemToGrid(UItem* Item, int32 Index);
	UOverlay* CreateItemOverlay(UItem* Item, const FVector2D& ItemOverlaySize, FItemButtonData ButtonData);
	UFUNCTION()
	UItemButtonWidget* CreateItemButton(UItem* Item, const FVector2D ItemButtonSize, FItemButtonData ButtonData);
	UTextBlock* CreateItemStackTextBlock(UItem* Item);
	UFUNCTION()
	void ItemTooltipShow(const FItemButtonData& ItemButtonData);
	UFUNCTION()
	void ItemTooltipHide();
	UFUNCTION()
	void ItemContextMenuShow(const FItemButtonData& ItemButtonData);
	UFUNCTION()
	void ItemContextMenuHide();
	UFUNCTION()
	void ItemDestory(const FItemButtonData& ItemButtonData);
};

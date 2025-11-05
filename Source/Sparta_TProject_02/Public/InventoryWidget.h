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
class UImage;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<UItemButtonWidget> DynamicButtonClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UItemButtonWidget* UseItemButtonWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UItemButtonWidget* DestroyItemButtonWidget;

	virtual void NativeConstruct() override;
public:
	UInventoryWidget(const FObjectInitializer& ObjectInitializer);
	void SetupWidget();
	bool RefreshWidget();
	bool AddItemToGrid(UItem* Item, int32 Index);
	UOverlay* CreateItemOverlay(UItem* Item, const FVector2D& ItemOverlaySize, FItemButtonData ButtonData);
	UFUNCTION()
	UItemButtonWidget* CreateItemButton(UItem* Item, UOverlay* Overlay, const FVector2D ItemButtonSize, FItemButtonData ButtonData);
	UTextBlock* CreateItemStackTextBlock(UItem* Item, UOverlay* Overlay);
	UImage* CreateItemImage(UItem* Item, UOverlay* Overlay);
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

	UInventory* GetInventory() const
	{
		return Inventory;
	}

	
};

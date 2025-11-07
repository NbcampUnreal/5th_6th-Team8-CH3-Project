#pragma once
#include "CoreMinimal.h"
#include "InventoryWidgetBase.h"
#include "EquipmentWidget.generated.h"

class UItem;
class UGem;
class UInventory;
class UGridPanel;
class UOverlay;
class UItemButtonWidget;
class UTextBlock;
class UImage;
class USizeBox;
struct FItemButtonData;

UCLASS()
class SPARTA_TPROJECT_02_API UEquipmentWidget : public UInventoryWidgetBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
	UInventory* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
	UInventory* GemSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
	UGridPanel* GemSlotsGrid;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
	UGridPanel* SelectGrid;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
	USizeBox* UnEquipBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
	UItemButtonWidget* UnEquipButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	int32 GridMaxColumn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TSubclassOf<UItemButtonWidget> DynamicButtonClass;


	
	virtual void NativeConstruct() override;

public:
	UEquipmentWidget(const FObjectInitializer& ObjectInitializer);
	void SetupWidget();
	virtual bool RefreshWidget() override;
	bool DisplayStatusInfo();
	bool AddGemToGrid(UItem* Item, int32 Index);
	
	UFUNCTION()
	UOverlay* CreateItemOverlay(UItem* Item, const FVector2D& ItemOverlaySize, FItemButtonData ButtonData);
	UFUNCTION()
	UItemButtonWidget* CreateItemButton(UItem* Item, const FVector2D ItemButtonSize, FItemButtonData ButtonData);
	UImage* CreateItemImage(UItem* Item, UOverlay* Overlay);
	UFUNCTION()
	void ItemTooltipShow(const FItemButtonData& ItemButtonData);
	UFUNCTION()
	void ItemTooltipHide();

	bool AddGemToSelectGrid(UItem* Item, int32 Index, int32 Column);
	UFUNCTION()
	UOverlay* CreateSelectItemOverlay(UItem* Item, const FVector2D& ItemOverlaySize, FItemButtonData ButtonData);
	UFUNCTION()
	void GemSelectShow(const FItemButtonData& ItemButtonData);
	UFUNCTION()
	void EquipmentSelectHide();
	UFUNCTION()
	void Equip(const FItemButtonData& ItemButtonData);
	UFUNCTION()
	void UnEquip(const FItemButtonData& ItemButtonData);
};

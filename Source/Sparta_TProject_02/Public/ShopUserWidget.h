// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h" 
#include "ShopItemEntryWidget.h"
#include "ShopItemData.h"
#include "ItemButtonWidget.h"
#include "ShopUserWidget.generated.h"

class UUserWidget;
class AShop;
struct FShopItemData;
class UInventory;
class UGridPanel;
class UOverlay;
class UImage;
struct FItemButtonData;

UCLASS()
class SPARTA_TPROJECT_02_API UShopUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton_X;


	AShop* ShopActor;

	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	void PopulateItemList(const TArray<FShopItemData>& ItemList);
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	void PopulateInventoryList();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ItemListScrollBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DescriptionTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UShopItemEntryWidget> ItemEntryWidgetClass;

	UPROPERTY(meta = (BindWidget))
	UGridPanel* InventoryGridPanel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UItemButtonWidget> DynamicButtonClass;

	int32 GridMaxColumn = 4;

private:
	UFUNCTION()
	void OnCloseButtonClicked();

	UFUNCTION()
	void UpdateDescriptionOnHover(const FShopItemData& ItemInfo);

	bool AddItemToGrid(class UItem* Item, int32 Index, UInventory* PlayerInventory);
	UOverlay* CreateItemOverlay(class UItem* Item, const FVector2D& ItemOverlaySize, FItemButtonData ButtonData);
	UItemButtonWidget* CreateItemButton(UItem* Item, UOverlay* Overlay, const FVector2D ItemButtonSize, FItemButtonData ButtonData);
	UTextBlock* CreateItemStackTextBlock(UItem* Item, UOverlay* Overlay);
	UImage* CreateItemImage(UItem* Item, UOverlay* Overlay);

public:
	UFUNCTION()
	void HandleItemCraftRequest(const FShopItemData& ItemInfo);
	UInventory* GetPlayerInventory() const;

};

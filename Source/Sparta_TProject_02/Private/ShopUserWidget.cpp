// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopUserWidget.h"
#include "Shop.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory.h"
#include "InventoryWidget.h"
#include "Engine/Engine.h"
#include "MyGameInstance.h"
#include "Components/GridPanel.h"
#include "Components/Overlay.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/GridSlot.h"
#include "ItemButtonWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "MaterialItem.h"
#include "Components/OverlaySlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UShopUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton_X)
	{
		CloseButton_X->OnClicked.AddDynamic(this, &UShopUserWidget::OnCloseButtonClicked);
	}

	/*if (HoverButton)
	{
		HoverButton->OnHovered.AddDynamic(this, &UShopUserWidget::OnItemButtonHovered);
	}*/

}

void UShopUserWidget::OnCloseButtonClicked()
{
	if (ShopActor == nullptr)
	{
		TArray<AActor*> FoundShops;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShop::StaticClass(), FoundShops);
		ShopActor = (FoundShops.Num() > 0) ? Cast<AShop>(FoundShops[0]) : nullptr;
	}

	if (ShopActor)
	{
		ShopActor->CloseShop();
	}
}

//void UShopUserWidget::OnItemButtonHovered()
//{
//	if (ShopActor == nullptr)
//	{
//		TArray<AActor*> FoundShops;
//		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShop::StaticClass(), FoundShops);
//		ShopActor = (FoundShops.Num() > 0) ? Cast<AShop>(FoundShops[0]) : nullptr;
//	}
//
//	if (ShopActor)
//	{
//		ShopActor->UpdateDescription();
//	}
//}

void UShopUserWidget::PopulateItemList(const TArray<FShopItemData>& ItemList)
{
	if (!ItemListScrollBox || !ItemEntryWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ScrollBox or ItemEntryWidgetClass is not set for Shop UI population!"));
		return;
	}

	ItemListScrollBox->ClearChildren();

	for (const FShopItemData& Item : ItemList)
	{
		UShopItemEntryWidget* ItemEntry = CreateWidget<UShopItemEntryWidget>(this, ItemEntryWidgetClass);
		if (ItemEntry)
		{
			ItemEntry->ShopItemData = Item;
			ItemEntry->SetItemEntryData(Item);

			// Hover 델리게이트에 상위 위젯의 함수 바인딩
			ItemEntry->OnItemHovered.AddDynamic(this, &UShopUserWidget::UpdateDescriptionOnHover);

			ItemEntry->OnItemCrafted.AddDynamic(this, &UShopUserWidget::HandleItemCraftRequest);

			ItemListScrollBox->AddChild(ItemEntry);
		}
	}
}

void UShopUserWidget::UpdateDescriptionOnHover(const FShopItemData& ItemInfo)
{
	UInventory* PlayerInventory = GetPlayerInventory();

	if (DescriptionTextBlock)
	{
		// 아이템 이름 및 설명/효과 섹션 구성
		FString Desc = FString::Printf(
			TEXT("아이템 이름: %s\n아이템 효과: %s\n\n레시피:\n"),
			*ItemInfo.ItemName.ToString(),
			*ItemInfo.FullDescription.ToString()
		);

		// 레시피 및 재료 정보 섹션 구성
		if (ItemInfo.bIsCraftable)
		{
			// FRecipeIngredient 구조체를 순회하며 재료 정보를 가져옴.
			for (const FRecipeIngredient& Ing : ItemInfo.Recipe)
			{
				// 재료 이름과 필요 개수 설정
				FString IngredientName = Ing.ItemID.ToString(); // 재료의 ItemID를 이름으로 사용
				int32 RequiredQuantity = Ing.Quantity;

				// 인벤토리에서 현재 보유 개수 조회
				int32 CurrentQuantity = 0;
				if (PlayerInventory)
				{
					// UInventory::GetItemQuantity 함수를 사용하여 현재 보유 개수를 가져옴.
					CurrentQuantity = PlayerInventory->GetItemQuantity(Ing.ItemID);
				}

				// 포맷팅 및 추가
				Desc += FString::Printf(
					TEXT("- %s (%d/%d)\n"),
					*IngredientName,
					CurrentQuantity,
					RequiredQuantity
				);
			}
		}
		else
		{
			// 제작 불가 아이템일 경우
			Desc += TEXT("- 제작 불가능한 아이템입니다.");
		}

		// TextBlock에 최종 텍스트 설정
		DescriptionTextBlock->SetText(FText::FromString(Desc));
	}
}

UInventory* UShopUserWidget::GetPlayerInventory() const
{
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (GameInstance)
	{
		UInventoryWidget* InventoryWidget = GameInstance->GetUIManager()->GetInventoryWidget();

		if (InventoryWidget)
		{
			if (InventoryWidget->GetInventory())
			{
				return InventoryWidget->GetInventory();
			}
		}
	}

	return nullptr;
}

void UShopUserWidget::HandleItemCraftRequest(const FShopItemData& ItemInfo)
{
	UInventory* PlayerInventory = GetPlayerInventory();

	if (!PlayerInventory)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("ERROR: Inventory Not Found/Accessible."));
		return;
	}

	if (ItemInfo.bIsCraftable)
	{
		bool bSuccess = PlayerInventory->TryCraftItem(ItemInfo.ItemID, ItemInfo.CraftingRecipe);

		if (bSuccess)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Crafting Successful")));
			PopulateInventoryList();
			UpdateDescriptionOnHover(ItemInfo);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Crafting Failed - Check Material or Inven Space.")));
		}
	}
	else
	{
		// 판매 기능 넣는다면.
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Buying Yet Made.")));
	}
}

void UShopUserWidget::PopulateInventoryList()
{
	UInventory* PlayerInventory = GetPlayerInventory();
	if (!InventoryGridPanel || !PlayerInventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shop Inventory Grid Panel or Inventory is not valid."));
		return;
	}

	// 기존 목록 제거
	InventoryGridPanel->ClearChildren();

	int32 ItemSize = PlayerInventory->GetCurrentSize();

	// 아이템을 Grid에 순회하며 추가
	for (int32 i = 0; i < ItemSize; ++i)
	{
		// AddItemToGrid는 이제 PlayerInventory 포인터를 받아야 함.
		if (!AddItemToGrid(PlayerInventory->GetItem(i), i, PlayerInventory))
		{
			// AddItemToGrid Error
			return;
		}
	}
}

bool UShopUserWidget::AddItemToGrid(class UItem* Item, int32 Index, UInventory* PlayerInventory)
{
	int32 Row = Index / GridMaxColumn;
	int32 Column = Index % GridMaxColumn;
	FVector2D ItemOverlaySize = FVector2D(100.0f, 100.0f); // 크기 설정 (블루프린트 크기에 맞춰 수정 가능)
	FMargin ItemMargin = FMargin(5.0f, 5.0f, 0.0f, 0.0f);

	// 참고: UInventoryWidget에서는 ButtonPosition 계산을 위해 부모 CanvasPanelSlot을 썼지만,
	// 상점 UI의 인벤토리 구역은 Tooltip이 필요 없다면 이 복잡한 계산은 생략해도 됩니다. 
	// 여기서는 ItemButtonData에 Position을 FVector2D::ZeroVector로 임시 설정합니다.

	FItemButtonData ButtonData;
	ButtonData.Index = Index;
	// 상점 인벤토리에서 툴팁을 띄우지 않는다면 ButtonPosition은 0으로 충분.
	ButtonData.ButtonPosition = FVector2D::ZeroVector;

	UOverlay* ItemOverlay = CreateItemOverlay(Item, ItemOverlaySize, ButtonData);
	if (!ItemOverlay) return false;

	UGridSlot* GridSlot = InventoryGridPanel->AddChildToGrid(Cast<UWidget>(ItemOverlay)); // InventoryGridPanel 사용

	GridSlot->SetPadding(ItemMargin);
	GridSlot->SetRow(Row);
	GridSlot->SetColumn(Column);

	return true;
}

UOverlay* UShopUserWidget::CreateItemOverlay(UItem* Item, const FVector2D& ItemOverlaySize, FItemButtonData ButtonData)
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	UOverlay* ItemOverlay = NewObject<UOverlay>(PC);
	if (!ItemOverlay) return nullptr;

	UItemButtonWidget* ItemButtonWidget = CreateItemButton(Item, ItemOverlay, ItemOverlaySize, ButtonData);
	if (!ItemButtonWidget) return nullptr;
	ItemOverlay->AddChild(Cast<UWidget>(ItemButtonWidget));

	// 아이템 이미지 추가
	UImage* Image = CreateItemImage(Item, ItemOverlay);
	if (Image)
	{
		UOverlaySlot* OverlaySlot = ItemOverlay->AddChildToOverlay(Image);
		OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}

	// 아이템 스택 수 텍스트 추가
	if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(Item))
	{
		UTextBlock* ItemStackTextBlock = CreateItemStackTextBlock(Item, ItemOverlay);
		if (!ItemStackTextBlock) return nullptr;
		ItemOverlay->AddChild(Cast<UWidget>(ItemStackTextBlock));

		UOverlaySlot* OverlaySlot = ItemOverlay->AddChildToOverlay(ItemStackTextBlock);
		OverlaySlot->SetPadding(FMargin(0.0f, 0.0f, 5.0f, 5.0f));
		OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
		OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
	}

	return ItemOverlay;
}

UItemButtonWidget* UShopUserWidget::CreateItemButton(UItem* Item, UOverlay* Overlay, const FVector2D ItemButtonSize, FItemButtonData ButtonData)
{
	if (!DynamicButtonClass) return nullptr;
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	UItemButtonWidget* ItemButtonWidget = CreateWidget<UItemButtonWidget>(PC, DynamicButtonClass);
	UButton* InnerButton = ItemButtonWidget->GetButton();
	ItemButtonWidget->SetupHoverBinding();
	ItemButtonWidget->SetupClickBinding();

	// Style 설정은 UInventoryWidget과 동일하게 유지
	// FDeprecateSlateVector2D ButtonSize = FDeprecateSlateVector2D(ItemButtonSize.X, ItemButtonSize.Y);
	// ... (스타일 설정 로직 복사) ...

	// 상점 인벤토리에서 툴팁/컨텍스트 메뉴는 필요 없거나 다르게 동작할 수 있으므로, 바인딩을 제거/수정합니다.
	// 예를 들어, 상점 인벤토리에서는 Hover시 아무것도 안 하거나, 클릭 시 아이템을 판매할 수 있습니다.
	/* ItemButtonWidget->OnHovered.AddDynamic(this, &UInventoryWidget::ItemTooltipShow); // 제거 또는 다른 함수 바인딩
	InnerButton->OnUnhovered.AddDynamic(this, &UInventoryWidget::ItemTooltipHide); // 제거
	ItemButtonWidget->OnClicked.AddDynamic(this, &UInventoryWidget::ItemContextMenuShow); // 제거 또는 다른 함수 바인딩
	*/

	ItemButtonWidget->SetButtonData(ButtonData);
	return ItemButtonWidget;
}

UTextBlock* UShopUserWidget::CreateItemStackTextBlock(UItem* Item, UOverlay* Overlay)
{
	UTextBlock* TextBlock = NewObject<UTextBlock>(Overlay);
	if (UMaterialItem* MaterialItem = Cast<UMaterialItem>(Item))
	{
		TextBlock->SetText(FText::FromString(FString::FromInt(MaterialItem->GetItemCurrentStack())));
	}
	else
	{
		TextBlock->SetText(FText::FromString(TEXT("1")));
	}
	return TextBlock;
}

UImage* UShopUserWidget::CreateItemImage(UItem* Item, UOverlay* Overlay)
{
	UImage* Image = NewObject<UImage>(Overlay);
	UTexture2D* ItemIcon = Item->GetItemIcon();
	if (!ItemIcon) return nullptr;

	Image->SetBrushFromTexture(ItemIcon, true);
	Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	return Image;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopUserWidget.h"
#include "Shop.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory.h"
#include "InventoryWidget.h"
#include "Engine/Engine.h"
#include "MyGameInstance.h"

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
		// 1. 아이템 이름 및 설명/효과 섹션 구성
		FString Desc = FString::Printf(
			TEXT("아이템 이름: %s\n아이템 효과: %s\n\n레시피:\n"),
			*ItemInfo.ItemName.ToString(),
			*ItemInfo.FullDescription.ToString()
		);

		// 2. 레시피 및 재료 정보 섹션 구성
		if (ItemInfo.bIsCraftable)
		{
			// FRecipeIngredient 구조체를 순회하며 재료 정보를 가져옴.
			for (const FRecipeIngredient& Ing : ItemInfo.Recipe)
			{
				// 3. 재료 이름과 필요 개수 설정
				FString IngredientName = Ing.ItemID.ToString(); // 재료의 ItemID를 이름으로 사용
				int32 RequiredQuantity = Ing.Quantity;

				// 4. 인벤토리에서 현재 보유 개수 조회
				int32 CurrentQuantity = 0;
				if (PlayerInventory)
				{
					// UInventory::GetItemQuantity 함수를 사용하여 현재 보유 개수를 가져옴.
					CurrentQuantity = PlayerInventory->GetItemQuantity(Ing.ItemID);
				}

				// 5. 포맷팅 및 추가
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

		// 6. TextBlock에 최종 텍스트 설정
		DescriptionTextBlock->SetText(FText::FromString(Desc));
	}
}

UInventory* UShopUserWidget::GetPlayerInventory() const
{
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (GameInstance)
	{
		UInventoryWidget* InventoryWidget = GameInstance->GetInventoryWidget();

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
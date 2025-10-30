// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopUserWidget.h"
#include "Shop.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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

			ItemListScrollBox->AddChild(ItemEntry);
		}
	}
}

void UShopUserWidget::UpdateDescriptionOnHover(const FShopItemData& ItemInfo)
{
	if (DescriptionTextBlock)
	{
		// 텍스트 포맷팅 및 설정 로직
		FString Desc = FString::Printf(TEXT("DESCRIPTION\n- %s\n\nRecipe:\n"), *ItemInfo.FullDescription.ToString());
		for (const FRecipeIngredient& Ing : ItemInfo.Recipe)
		{
			Desc += FString::Printf(TEXT("- %s x%d\n"), *Ing.ItemID.ToString(), Ing.Quantity);
		}
		DescriptionTextBlock->SetText(FText::FromString(Desc));
	}
}
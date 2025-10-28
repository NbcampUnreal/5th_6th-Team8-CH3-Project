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

	if (HoverButton)
	{
		HoverButton->OnHovered.AddDynamic(this, &UShopUserWidget::OnItemButtonHovered);
	}

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

void UShopUserWidget::OnItemButtonHovered()
{
	if (ShopActor == nullptr)
	{
		TArray<AActor*> FoundShops;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShop::StaticClass(), FoundShops);
		ShopActor = (FoundShops.Num() > 0) ? Cast<AShop>(FoundShops[0]) : nullptr;
	}

	if (ShopActor)
	{
		ShopActor->UpdateDescription();
	}
}
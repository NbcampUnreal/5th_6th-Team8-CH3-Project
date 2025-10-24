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

}

void UShopUserWidget::OnCloseButtonClicked()
{
	TArray<AActor*> FoundShops;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShop::StaticClass(), FoundShops);
	AShop* ShopActor = (FoundShops.Num() > 0) ? Cast<AShop>(FoundShops[0]) : nullptr;

	if (ShopActor)
	{
		ShopActor->CloseShop();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("ShopUI Closed"));
		}
	}
}
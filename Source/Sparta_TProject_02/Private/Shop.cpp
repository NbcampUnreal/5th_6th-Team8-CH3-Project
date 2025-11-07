// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ShopUserWidget.h"

// Sets default values
AShop::AShop()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Creating ShopUI Instance
	ShopWidgetClass = nullptr;
	ShopWidgetInstance = nullptr;
	ShopVisible = false;
	//CraftableList = {};
	//SelectedItem = nullptr;
}

// Called when the game starts or when spawned
void AShop::BeginPlay()
{
	Super::BeginPlay();

	//SHOP WIDGET CREATION
	if (ShopWidgetInstance)
	{
		ShopWidgetInstance->RemoveFromParent();
		ShopWidgetInstance = nullptr;
	}

	if (ShopWidgetClass)
	{
		ShopWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ShopWidgetClass);
		if (ShopWidgetInstance)
		{
			ShopWidgetInstance->AddToViewport(100);
			ShopWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	/*CraftableList = {

	};*/

	//PlayerInventory = ;

}

//ShopUI
void AShop::OpenShop()
{
	if (ShopWidgetInstance)
	{
		ShopWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		ShopVisible = true;

		UShopUserWidget* ShopWidget = Cast<UShopUserWidget>(ShopWidgetInstance);

		if (ShopWidget)
		{
			ShopWidget->PopulateItemList(AvailableItems);

			// 상점 인벤토리 목록 갱신 호출
			ShopWidget->PopulateInventoryList();
		}

		if (PC)
		{
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Shop Opened Successfully!"));
		}
	}
}

void AShop::CloseShop()
{
	if (ShopWidgetInstance)
	{
		ShopWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		ShopVisible = false;
		if (PC)
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}

void AShop::UpdateList()
{
	UShopUserWidget* ShopWidget = Cast<UShopUserWidget>(ShopWidgetInstance);

	if (ShopWidget)
	{
		// 상점 인벤토리 목록 갱신 호출 
		ShopWidget->PopulateInventoryList();

		// (선택 사항) 만약 상점 목록도 재료 개수 변화 때문에 갱신이 필요하다면 아래를 호출 - 추후 이런 기능 도입 한다면 -
		// ShopWidget->PopulateItemList(AvailableItems);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Shop and Inventory Lists Updated."));
		}
	}
}

void AShop::UpdateDescription()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("UpdateDescription():Shop.cpp Called."));
	}
}

void AShop::SetShopPlayerController(APlayerController* MyPlayerController)
{
	PC = MyPlayerController;
}
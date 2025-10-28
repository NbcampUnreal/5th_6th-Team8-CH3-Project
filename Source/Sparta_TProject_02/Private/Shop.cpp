// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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
			ShopWidgetInstance->AddToViewport();
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
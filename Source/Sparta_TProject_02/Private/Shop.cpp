// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AShop::AShop()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CraftableList = {};
	PlayerInventory = nullptr;
	ShopWidgetClass = nullptr;
	ShopWidgetInstance = nullptr;
}

// Called when the game starts or when spawned
void AShop::BeginPlay()
{
	Super::BeginPlay();
	
	CraftableList = {

	};

	//PlayerInventory = ;

	if (ShopWidgetClass)
	{
		ShopWidgetInstance = CreateWidget<UUserWidget>(this, ShopWidgetClass);
		if (ShopWidgetInstance)
		{
			ShopWidgetInstance->AddToViewport();
		}
	}

}
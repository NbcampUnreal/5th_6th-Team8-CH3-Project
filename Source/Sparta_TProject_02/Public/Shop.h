// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory.h"
#include "Item.h"
#include "Shop.generated.h"

class Item;
class Inventory;

UCLASS()
class SPARTA_TPROJECT_02_API AShop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<Item> CraftableList;
	Inventory* PlayerInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TSubclassOf<UUserWidget> ShopWidgetClass;
	UUserWidget* ShopWidgetInstance;


public:	
	void OpenShop();
	void CloseShop();
	void UpdateList();
	void UpdateDescription();
	TArray<Item>* GetCraftableList();

};

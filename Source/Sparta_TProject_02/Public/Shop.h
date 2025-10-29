// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory.h"
#include "Item.h"
#include "ShopItemData.h"
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

	APlayerController* PC;

	//SHOP WIDGET
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TSubclassOf<UUserWidget> ShopWidgetClass;
	UUserWidget* ShopWidgetInstance;
	bool ShopVisible;

	//ITEM MANAGEMENT
	/*UPROPERTY(BlueprintReadOnly, Category = "Shop")
	UItem* SelectedItem;*/

public:	
	void SetShopPlayerController(APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void OpenShop();
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void CloseShop();
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UpdateList();
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UpdateDescription();

	//UFUNCTION(BlueprintPure, Category = "Shop")
	//TArray<Item>* GetCraftableList();

	/*UFUNCTION(BlueprintCallable, Category = "Shop")
	void OnItemHovered(UItem* HoveredItem);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void OnBuyButtonClicked(UItem* SelectedItem);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void AddItemToInventory(UItem* SelectedItem, ACharacter* PlayerCharacter);*/

private:
	UPROPERTY(EditAnywhere, Category = "Shop Data")
	TArray<FShopItemData> AvailableItems;
};

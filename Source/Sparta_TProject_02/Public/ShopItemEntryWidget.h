// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"             // Delegate 기본 정의
#include "Delegates/DelegateCombinations.h" // DYNAMIC MULTICAST 델리게이트 정의 포함
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ShopItemData.h"
#include "ShopItemEntryWidget.generated.h"

class UButton;

UCLASS()
class SPARTA_TPROJECT_02_API UShopItemEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FShopItemData ShopItemData;

	// Hover 이벤트 시 FItemData를 상위 위젯에 전달하기 위한 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemHoveredSignature, const FShopItemData&, ItemInfo);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemHoveredSignature OnItemHovered;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemCraftedSignature, const FShopItemData&, ItemInfo);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnItemCraftedSignature OnItemCrafted;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* ItemButton; // WBP_ShopItemEntry에서 버튼의 이름과 일치해야 함

	UPROPERTY(meta = (BindWidget))
	UButton* CraftButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleItemButtonHovered();

	UFUNCTION()
	void HandleCraftButtonClicked();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "ShopUI")
	void SetItemEntryData(const FShopItemData& Data);
};

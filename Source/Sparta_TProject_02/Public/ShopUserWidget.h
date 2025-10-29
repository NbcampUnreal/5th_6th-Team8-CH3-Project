// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h" 
#include "ShopItemEntryWidget.h"
#include "ShopItemData.h"
#include "ShopUserWidget.generated.h"

class UUserWidget;
class AShop;

UCLASS()
class SPARTA_TPROJECT_02_API UShopUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton_X;
	/*UPROPERTY(meta = (BindWidget))
	UButton* HoverButton;*/

	AShop* ShopActor;

	// AShop에서 호출하여 목록을 채우는 함수
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	void PopulateItemList(const TArray<FShopItemData>& ItemList);

protected:
	virtual void NativeConstruct() override;

	// WBP_ShopUI에 있는 Scroll Box 이름과 일치해야 합니다.
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ItemListScrollBox;

	// WBP_ShopUI에 있는 Description 표시용 TextBlock 이름과 일치해야 합니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DescriptionTextBlock;

	// WBP_ItemEntry 블루프린트 클래스를 지정할 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UShopItemEntryWidget> ItemEntryWidgetClass;

private:
	UFUNCTION()
	void OnCloseButtonClicked();
	/*UFUNCTION()
	void OnItemButtonHovered();*/

	// 항목 위젯의 Hover 이벤트를 수신하여 상세 설명을 업데이트하는 함수
	UFUNCTION()
	void UpdateDescriptionOnHover(const FShopItemData& ItemInfo);

};

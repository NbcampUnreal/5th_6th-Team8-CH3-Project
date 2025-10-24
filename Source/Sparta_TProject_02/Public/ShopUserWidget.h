// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ShopUserWidget.generated.h"

class UUserWidget;

UCLASS()
class SPARTA_TPROJECT_02_API UShopUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton_X;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnCloseButtonClicked();

};

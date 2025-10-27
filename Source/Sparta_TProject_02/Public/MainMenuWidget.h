// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPARTA_TPROJECT_02_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    UButton* StartButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ExitButton;

    UFUNCTION()
    void HandleStartClicked();

    UFUNCTION()
    void HandleExitClicked();

};

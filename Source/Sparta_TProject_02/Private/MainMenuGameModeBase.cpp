// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (ShowMainMenu)
    {
        CurrentMenuWidget = CreateWidget<UUserWidget>(GetWorld(), ShowMainMenu);
        if (CurrentMenuWidget)
        {
            CurrentMenuWidget->AddToViewport();

            APlayerController* PC = GetWorld()->GetFirstPlayerController();
            if (PC)
            {
                PC->SetShowMouseCursor(true);
                PC->SetInputMode(FInputModeUIOnly());
            }
        }
    }
}

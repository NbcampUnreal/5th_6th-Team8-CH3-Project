#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton)
        StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleStartClicked);

    if (ExitButton)
        ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleExitClicked);
}

void UMainMenuWidget::HandleStartClicked()
{
    // Start구현
    UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked!"));
}

void UMainMenuWidget::HandleExitClicked()
{
    APlayerController* PC = GetOwningPlayer();
    UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
}

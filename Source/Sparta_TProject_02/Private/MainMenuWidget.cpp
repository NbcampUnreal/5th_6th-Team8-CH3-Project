#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
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
    UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked!"));

    //텍스트에 적힌 이름으로 이동
    UGameplayStatics::OpenLevel(this, FName("Temp_Level"));
}

void UMainMenuWidget::HandleExitClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Exit Button Clicked!"));
    //게임종료
    APlayerController* PC = GetOwningPlayer();
    UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
}

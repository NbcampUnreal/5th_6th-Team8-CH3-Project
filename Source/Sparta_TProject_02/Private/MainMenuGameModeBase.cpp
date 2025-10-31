#include "MainMenuGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void AMainMenuGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (ShowMainMenu)
    {
        CurrentMenuWidget = CreateWidget<UUserWidget>(GetWorld(), ShowMainMenu);
        if (CurrentMenuWidget)
        {
            CurrentMenuWidget->AddToViewport();

            //입력 모드 세팅
            APlayerController* PC = GetWorld()->GetFirstPlayerController();
            if (PC)
            {
                // 커서 보이게
                PC->SetShowMouseCursor(true);

                // UI 입력 전용 모드로 설정
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(CurrentMenuWidget->TakeWidget());
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                PC->SetInputMode(InputMode);
            }
        }
    }
}

#include "UIManager.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.h"
#include "GameHUDWidget.h"

void UUIManager::Init(APlayerController* InPC)
{
    PC = InPC;
}

void UUIManager::ShowMainMenu()
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    if (MainMenuClass)
    {
        MainMenuWidget = CreateWidget<UMainMenuWidget>(PC, MainMenuClass);
        CurrentWidget = MainMenuWidget;
        CurrentWidget->AddToViewport();
    }
}

void UUIManager::ShowGameHUD()
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    if (GameHUDClass)
    {
        GameHUDWidget = CreateWidget<UGameHUDWidget>(PC, GameHUDClass);
        CurrentWidget = GameHUDWidget;
        CurrentWidget->AddToViewport();
    }
}

void UUIManager::UpdateHealth(float NewHP)
{
    if (GameHUDWidget)
    {
        GameHUDWidget->SetHealth(NewHP);
    }
}

void UUIManager::UpdateWaveInfo(int32 CurrentWave, float TimeToNext)
{
    if (GameHUDWidget)
    {
        GameHUDWidget->SetWaveInfo(CurrentWave, TimeToNext);
    }
}

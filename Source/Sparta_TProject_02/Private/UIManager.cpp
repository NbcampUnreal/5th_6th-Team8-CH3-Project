#include "UIManager.h"
#include "Blueprint/UserWidget.h"
#include "GameHUDWidget.h"
//#include "ShopWidget.h"
#include "GameFramework/PlayerController.h"

void UUIManager::Init(APlayerController* InPC)
{
    PC = InPC;

    // 기본 HUD 생성
    if (GameHUDClass && PC)
    {
        GameHUD = CreateWidget<UGameHUDWidget>(PC, GameHUDClass);
        if (GameHUD)
        {
            GameHUD->AddToViewport(0); // 레벨 0: 항상 기본 레이어
        }
    }
}

// --- HUD는 항상 유지 ---
void UUIManager::ShowHUD()
{
    if (GameHUD && !GameHUD->IsInViewport())
    {
        GameHUD->AddToViewport(0);
    }
}

// --- 상점 UI 열기 ---
void UUIManager::ShowShop()
{
    if (CurrentPopup)
        return; // 이미 다른 팝업이 떠있으면 무시

    if (ShopWidgetClass && PC)
    {
        CurrentPopup = CreateWidget<UUserWidget>(PC, ShopWidgetClass);
        if (CurrentPopup)
        {
            CurrentPopup->AddToViewport(1); // 레벨 1: HUD 위에 표시
            PC->SetInputMode(FInputModeUIOnly()); // UI 조작 모드로 전환
            PC->bShowMouseCursor = true;
        }
    }
}

// --- 상점 UI 닫기 ---
void UUIManager::HideShop()
{
    if (CurrentPopup)
    {
        CurrentPopup->RemoveFromParent();
        CurrentPopup = nullptr;

        // 다시 게임 모드로 복귀
        if (PC)
        {
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
        }
    }
}

// --- 게임오버 UI 표시 ---
void UUIManager::ShowGameOver()
{
    if (CurrentPopup)
        CurrentPopup->RemoveFromParent();

    if (GameOverWidgetClass && PC)
    {
        CurrentPopup = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
        if (CurrentPopup)
        {
            CurrentPopup->AddToViewport(1);
        }
    }
}

// --- 전체 UI 제거 ---
void UUIManager::ClearAllUI()
{
    if (GameHUD)
        GameHUD->RemoveFromParent();

    if (CurrentPopup)
    {
        CurrentPopup->RemoveFromParent();
        CurrentPopup = nullptr;
    }
}

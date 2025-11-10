#include "UIManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameHUDWidget.h"
//#include "ShopWidget.h"
#include "GameFramework/PlayerController.h"
#include "InventoryWidget.h"
#include "EquipmentWidget.h"
#include "Components/Widget.h"
#include "MyGameInstance.h"

UUIManager::UUIManager()
{
   GameOverWidgetClass = nullptr;
   GameOverWidgetInstance = nullptr;

   InventoryWidgetClass = nullptr;
   InventoryWidgetInstance = nullptr;

   EquipmentWidgetClass = nullptr;
   EquipmentWidgetInstance = nullptr;

   static ConstructorHelpers::FClassFinder<UInventoryWidget> InvenHUDFInder(
      TEXT("/Game/Blueprints/WBP_InventoryWidget.WBP_InventoryWidget_C"));
   if (InvenHUDFInder.Succeeded())
   {
      InventoryWidgetClass = InvenHUDFInder.Class;
   }
   static ConstructorHelpers::FClassFinder<UEquipmentWidget> EquipHUDFInder(
      TEXT("/Game/Blueprints/WBP_EquipmentWidget.WBP_EquipmentWidget_C"));
   if (EquipHUDFInder.Succeeded())
   {
      EquipmentWidgetClass = EquipHUDFInder.Class;
   }
}

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
//void UUIManager::ShowGameOver()
//{
//    if (CurrentPopup)
//        CurrentPopup->RemoveFromParent();
//
//    if (GameOverWidgetClass && PC)
//    {
//        CurrentPopup = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
//        if (CurrentPopup)
//        {
//            CurrentPopup->AddToViewport(1);
//        }
//    }
//}

void UUIManager::SetupGameOverWidget()
{
   if (!GameOverWidgetClass) return;
   GameOverWidgetInstance = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
}
void UUIManager::ShowGameOver()
{
   //UButton* TitleButton = Cast<UButton>(GetWidgetFromName("TitleButton"));
   //UButton* ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("ExitButton")));
   if (!GameOverWidgetInstance) return;
   GameOverWidgetInstance->AddToViewport(5);
   PC->SetInputMode(FInputModeUIOnly()); // UI 조작 모드로 전환
   PC->bShowMouseCursor = true;
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


void UUIManager::SetupInventoryWidget()
{
   if (!InventoryWidgetClass) return;

   InventoryWidgetInstance = CreateWidget<UInventoryWidget>(PC, InventoryWidgetClass);
   if (!InventoryWidgetInstance) return;

   if (!PC) return;
   if (!PC->GetGameInstance()) return;

   UMyGameInstance* GameInstance = Cast<UMyGameInstance>(PC->GetGameInstance());
   if (!GameInstance) return;

   UInventory* Inventory = GameInstance->GetInventory();
   if (!Inventory) return;
   Inventory->SetLinkedWidget(InventoryWidgetInstance);

   InventoryWidgetInstance->AddToViewport(4);
   InventoryWidgetInstance->SetupWidget();
   InventoryWidgetInstance->ItemTooltipHide();
   InventoryWidgetInstance->ItemContextMenuHide();
}

void UUIManager::SetupEquipmentWidget()
{
   if (!EquipmentWidgetClass) return;
   EquipmentWidgetInstance = CreateWidget<UEquipmentWidget>(PC, EquipmentWidgetClass);
   if (!EquipmentWidgetInstance) return;

   if (!PC) return;
   if (!PC->GetGameInstance()) return;

   UMyGameInstance* GameInstance = Cast<UMyGameInstance>(PC->GetGameInstance());
   if (!GameInstance) return;

   UInventory* GemSlots = GameInstance->GetGemSlots();
   if (!GemSlots) return;
   GemSlots->SetLinkedWidget(EquipmentWidgetInstance);

   EquipmentWidgetInstance->AddToViewport(3);
   EquipmentWidgetInstance->SetupWidget();
   EquipmentWidgetInstance->ItemTooltipHide();
   EquipmentWidgetInstance->EquipmentSelectHide();
}

void UUIManager::OpenInventoryWidget()
{
   InventoryWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
   InventoryWidgetInstance->RefreshWidget();
}
void UUIManager::CloseInventoryWidget()
{
   InventoryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
}

void UUIManager::OpenEquipmentWidget()
{
   EquipmentWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
   EquipmentWidgetInstance->RefreshWidget();
}
void UUIManager::CloseEquipmentWidget()
{
   EquipmentWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
}

void UUIManager::ToggleInventoryWidget()
{
   if (InventoryWidgetInstance->GetVisibility() == ESlateVisibility::Collapsed)
   {
      OpenInventoryWidget();
      OpenEquipmentWidget();
      GetInventoryWidget()->ItemContextMenuHide();
      FInputModeGameAndUI InputMode;
      InputMode.SetWidgetToFocus(nullptr);
      InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
      PC->SetInputMode(InputMode);
      PC->bShowMouseCursor = true;
   }
   else
   {
      CloseInventoryWidget();
      CloseEquipmentWidget();
      PC->SetInputMode(FInputModeGameOnly());
      PC->bShowMouseCursor = false;
   }
}

UInventoryWidget* UUIManager::GetInventoryWidget() const
{
   return InventoryWidgetInstance;
}

UEquipmentWidget* UUIManager::GetEquipmentWidget() const
{
   return EquipmentWidgetInstance;
}

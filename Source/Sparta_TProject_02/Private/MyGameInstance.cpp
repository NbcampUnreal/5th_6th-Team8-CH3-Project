#include "MyGameInstance.h"
#include "InventoryWidget.h"
<<<<<<< HEAD
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
=======
#include "EquipmentWidget.h"
>>>>>>> feature/ItemJKH
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerCharacterController.h"

UMyGameInstance::UMyGameInstance()
{
    // 레벨 이름
    MainMenuLevelName = FName("MainMenu_Map");
    GameLevelName = FName("Game_Map");

    // 점수 초기화
    HighScore = 0;

   Inventory = nullptr;
   InventoryWidgetClass = nullptr;
   InventoryWidgetInstance = nullptr;
	static ConstructorHelpers::FClassFinder<UInventoryWidget> InvenHUDFInder(
		TEXT("/Game/Blueprints/WBP_InventoryWidget.WBP_InventoryWidget_C"));
   if (InvenHUDFInder.Succeeded())
   {
      InventoryWidgetClass = InvenHUDFInder.Class;
   }

   GemSlots = nullptr;
   EquipmentWidgetClass = nullptr;
   EquipmentWidgetInstance = nullptr;
   static ConstructorHelpers::FClassFinder<UEquipmentWidget> EquipHUDFInder(
      TEXT("/Game/Blueprints/WBP_EquipmentWidget.WBP_EquipmentWidget_C"));
   if (EquipHUDFInder.Succeeded())
   {
      EquipmentWidgetClass = EquipHUDFInder.Class;
   }
}

UInventoryWidget* UMyGameInstance::GetInventoryWidget() const
{
   return InventoryWidgetInstance;
}

void UMyGameInstance::Init()
{
   Super::Init();
}

// ===== 게임 흐름 함수들 =====
void UMyGameInstance::LoadMainMenu()
{
    if (MainMenuLevelName == NAME_None)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameInstance] MainMenuLevelName not set!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("[GameInstance] Loading Main Menu..."));
    UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
}

void UMyGameInstance::LoadGameLevel()
{
    if (GameLevelName == NAME_None)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameInstance] GameLevelName not set!"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("[GameInstance] Loading Game Level..."));
    UGameplayStatics::OpenLevel(GetWorld(), GameLevelName);
}

void UMyGameInstance::RestartCurrentLevel()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()));
}

void UMyGameInstance::QuitGame()
{
    UKismetSystemLibrary::QuitGame(
        GetWorld(),
        GetWorld()->GetFirstPlayerController(),
        EQuitPreference::Quit,
        false
    );
}

// ===== 점수 함수들 =====
void UMyGameInstance::SetNewHighScore(int32 NewScore)
{
    if (NewScore > HighScore)
    {
        HighScore = NewScore;
        UE_LOG(LogTemp, Log, TEXT("[GameInstance] New High Score: %d"), HighScore);
    }
}

int32 UMyGameInstance::GetHighScore() const
{
    return HighScore;
}

void UMyGameInstance::SetupInventoryWidget(APlayerCharacterController* PlayerContorller)
{
   if (!PlayerContorller) return;
   Inventory = NewObject<UInventory>(PlayerContorller);
   if (!InventoryWidgetClass) return;
   InventoryWidgetInstance = CreateWidget<UInventoryWidget>(PlayerContorller, InventoryWidgetClass);

   InventoryWidgetInstance->AddToViewport();
   InventoryWidgetInstance->SetupWidget();
   InventoryWidgetInstance->ItemTooltipHide();
   InventoryWidgetInstance->ItemContextMenuHide();
}

void UMyGameInstance::SetupEquipmentWidget(APlayerCharacterController* PlayerContorller)
{
   if (!PlayerContorller) return;
   GemSlots = NewObject<UInventory>(PlayerContorller);

   if (!EquipmentWidgetClass) return;
   EquipmentWidgetInstance = CreateWidget<UEquipmentWidget>(PlayerContorller, EquipmentWidgetClass);
   GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("EquipmentWidgetClass Success"));

   EquipmentWidgetInstance->AddToViewport();
   EquipmentWidgetInstance->SetupWidget();
   EquipmentWidgetInstance->ItemTooltipHide();
   EquipmentWidgetInstance->EquipmentSelectHide();
}
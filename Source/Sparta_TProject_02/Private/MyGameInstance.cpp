#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerCharacterController.h"

UMyGameInstance::UMyGameInstance()
{
    // 레벨 이름
    MainMenuLevelName = FName("MainMenu_Map");
    GameLevelName = FName("Game_Map");

    // 점수 초기화
    HighScore = 0;


   UiManagerClass = nullptr;
   UiManagerInstance = nullptr;
   
   Inventory = nullptr;
   GemSlots = nullptr;
   
   static ConstructorHelpers::FClassFinder<UInventory> InventoryClassFinder(
      TEXT("/Game/Blueprints/BPC_Inventory.BPC_Inventory_C"));
   if (InventoryClassFinder.Succeeded())
   {
      InventoryBlueprintClass = InventoryClassFinder.Class;
   }
}

void UMyGameInstance::Init()
{
   Super::Init();
}

void UMyGameInstance::SetupUIManager(APlayerController* PlayerContorller)
{
   if (UiManagerClass)
   {
      UiManagerInstance = NewObject<UUIManager>(PlayerContorller, UiManagerClass);
   }
   else
   {
      UiManagerInstance = NewObject<UUIManager>(PlayerContorller, UUIManager::StaticClass());
   }
   UiManagerInstance->Init(PlayerContorller);
}

void UMyGameInstance::SetupInventroyAndEquipment(APlayerController* PlayerContorller)
{
   if (Inventory)
   {
      while (!Inventory->IsEmpty())
      {
         Inventory->RemoveItemIndex(0);
      }
   }
   if (GemSlots)
   {
      while (!GemSlots->IsEmpty())
      {
         GemSlots->RemoveItemIndex(0);
      }
   }

   if (!PlayerContorller) return;
   UClass* ClassToSpawn = InventoryBlueprintClass.Get() ? InventoryBlueprintClass.Get() : UInventory::StaticClass();
   Inventory = NewObject<UInventory>(PlayerContorller, ClassToSpawn);
   GemSlots = NewObject<UInventory>(PlayerContorller);
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
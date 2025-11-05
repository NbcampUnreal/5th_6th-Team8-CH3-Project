#include "MyGameInstance.h"
#include "InventoryWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
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

   static ConstructorHelpers::FClassFinder<UInventory> InventoryClassFinder(
	   TEXT("/Game/Blueprints/BPC_Inventory.BPC_Inventory_C"));
   if (InventoryClassFinder.Succeeded())
   {
	   InventoryBlueprintClass = InventoryClassFinder.Class;
   }

	static ConstructorHelpers::FClassFinder<UInventoryWidget> InvenHUDFInder(
		TEXT("/Game/Blueprints/WBP_InventoryWidget.WBP_InventoryWidget_C"));
   if (InvenHUDFInder.Succeeded())
   {
      InventoryWidgetClass = InvenHUDFInder.Class;
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

	UClass* ClassToSpawn = InventoryBlueprintClass.Get() ? InventoryBlueprintClass.Get() : UInventory::StaticClass();
	Inventory = NewObject<UInventory>(PlayerContorller, ClassToSpawn);

	//Inventory = NewObject<UInventory>(PlayerContorller);

	if (!InventoryWidgetClass) return;
	InventoryWidgetInstance = CreateWidget<UInventoryWidget>(PlayerContorller, InventoryWidgetClass);

	InventoryWidgetInstance->AddToViewport();
	InventoryWidgetInstance->SetupWidget();

	InventoryWidgetInstance->SetVisibility(ESlateVisibility::Visible);

	InventoryWidgetInstance->ItemTooltipHide();
	InventoryWidgetInstance->ItemContextMenuHide();
}
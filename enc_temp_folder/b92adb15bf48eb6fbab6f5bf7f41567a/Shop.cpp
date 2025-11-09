#include "Shop.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ShopUserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Engine.h"

AShop::AShop()
{
	PrimaryActorTick.bCanEverTick = false;
	ShopWidgetClass = nullptr;
	ShopWidgetInstance = nullptr;
	ShopVisible = false;
	PC = nullptr;
}

void AShop::BeginPlay()
{
	Super::BeginPlay();

	if (ShopWidgetInstance)
	{
		ShopWidgetInstance->RemoveFromParent();
		ShopWidgetInstance = nullptr;
	}

	if (ShopWidgetClass)
	{
		ShopWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ShopWidgetClass);
		if (ShopWidgetInstance)
		{
			ShopWidgetInstance->AddToViewport(100);
			ShopWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!PC)
	{
		PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}
}

void AShop::OpenShop()
{
	if (!ShopWidgetInstance || !PC) return;

	ShopWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	ShopVisible = true;

	if (UShopUserWidget* ShopWidget = Cast<UShopUserWidget>(ShopWidgetInstance))
	{
		ShopWidget->PopulateItemList(AvailableItems);
		ShopWidget->PopulateInventoryList();
	}

	PC->bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ShopWidgetInstance->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Shop Opened: Mouse + UI Mode Active"));
}

void AShop::CloseShop()
{
	if (!ShopWidgetInstance || !PC) return;

	ShopWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	ShopVisible = false;

	PC->bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);

	UWidgetBlueprintLibrary::SetFocusToGameViewport();

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Shop Closed: Game Mode Restored"));
}

void AShop::UpdateList()
{
	if (UShopUserWidget* ShopWidget = Cast<UShopUserWidget>(ShopWidgetInstance))
	{
		ShopWidget->PopulateInventoryList();
	}
}

void AShop::UpdateDescription()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("UpdateDescription(): Shop.cpp Called."));
	}
}

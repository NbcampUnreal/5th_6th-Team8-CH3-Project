#include "PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyGameInstance.h"
#include "Engine/LocalPlayer.h"

APlayerCharacterController::APlayerCharacterController()
	: InputMappingContext(nullptr)
{
	bIsInUIOnlyMode = false;
}

void APlayerCharacterController::TriggerStageReward()
{
	UE_LOG(LogTemp, Log, TEXT("TriggerStageReward Called"));
}

void APlayerCharacterController::OnGameOver()
{
	UE_LOG(LogTemp, Log, TEXT("OnGameOver_Implementation Called"));
}

void APlayerCharacterController::OnGameWin()
{
	UE_LOG(LogTemp, Log, TEXT("OnGameWin_Implementation Called"));
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(GetLocalPlayer()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SetupInventroyAndEquipment(this);
		GameInstance->SetupUIManager(this);
		GameInstance->GetUIManager()->SetupInventoryWidget();
		GameInstance->GetUIManager()->SetupEquipmentWidget();
		GameInstance->GetUIManager()->CloseInventoryWidget();
		GameInstance->GetUIManager()->CloseEquipmentWidget();
	}
}

void APlayerCharacterController::ToggleUIInput()
{
	// 현재 입력 모드를 추적하기 위한 변수가 있다고 가정
	if (bIsInUIOnlyMode) // UI Only 모드라면 게임 모드로 복귀
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
		bIsInUIOnlyMode = false;
	}
	else // 게임 모드라면 UI Only 모드로 전환
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(nullptr); // 포커스를 줄 위젯이 없다면 nullptr
		SetInputMode(InputMode);
		bShowMouseCursor = true;
		bIsInUIOnlyMode = true;
	}
}

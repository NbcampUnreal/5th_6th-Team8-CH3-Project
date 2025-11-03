#include "PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

APlayerCharacterController::APlayerCharacterController()
	: InputMappingContext(nullptr)
{
}

void APlayerCharacterController::TriggerStageReward()
{
	//À§Á¬ °»½Å
	UE_LOG(LogTemp, Log, TEXT("TriggerStageReward Called"));
}

void APlayerCharacterController::OnGameOver()
{
	//À§Á¬ °»½Å
	UE_LOG(LogTemp, Log, TEXT("OnGameOver_Implementation Called"));
}

void APlayerCharacterController::OnGameWin()
{
	//À§Á¬ °»½Å
	UE_LOG(LogTemp, Log, TEXT("OnGameWin_Implementation Called"));
}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}



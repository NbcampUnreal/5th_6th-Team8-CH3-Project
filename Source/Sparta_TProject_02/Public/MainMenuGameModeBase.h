// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SPARTA_TPROJECT_02_API AMainMenuGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> ShowMainMenu;

private:
	UUserWidget* CurrentMenuWidget;
};

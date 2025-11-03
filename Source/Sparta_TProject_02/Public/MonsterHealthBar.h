// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class SPARTA_TPROJECT_02_API UMonsterHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void UpdateHealthBar(float HealthRatio);
};

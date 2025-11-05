#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Inventory.h"
#include "Components/Button.h"
#include "MyGameInstance.generated.h"

class UUserWidget;
class UGridPanel;
struct FItemButtonData;
class UInventoryWidget;
class UEquipmentWidget;
class APlayerCharacterController;

UCLASS()
class SPARTA_TPROJECT_02_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryWidget* InventoryWidgetInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<UEquipmentWidget> EquipmentWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	UEquipmentWidget* EquipmentWidgetInstance;

	virtual void Init() override;

public:
	UMyGameInstance();
	// ===== 게임 흐름 관리 =====
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void LoadGameLevel();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void RestartCurrentLevel();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void QuitGame();

	// ===== 점수 관리 =====
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetNewHighScore(int32 NewScore);

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetHighScore() const;

	//아이템
	UInventoryWidget* GetInventoryWidget() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventory* Inventory;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
	UInventory* GemSlots;

	void SetupInventoryWidget(APlayerCharacterController* PlayerContorller);
	void SetupEquipmentWidget(APlayerCharacterController* PlayerContorller);

	//Shop 연동용
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventory> InventoryBlueprintClass;

protected:
	// ===== 레벨 관련 =====
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Levels")
	FName MainMenuLevelName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Levels")
	FName GameLevelName;

	// ===== 점수 관련 =====
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 HighScore;

};

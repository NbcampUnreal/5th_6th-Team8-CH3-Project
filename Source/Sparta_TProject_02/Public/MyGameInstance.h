#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Inventory.h"
#include "UIManager.h"
#include "MyGameInstance.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUIManager> UiManagerClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUIManager* UiManagerInstance;

	//아이템
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventory* Inventory;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
	UInventory* GemSlots;

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

	// UI
	UUIManager* GetUIManager() const { return UiManagerInstance; };
	// Item (Inventory and Equipment)
	void SetupUIManager(APlayerController* PlayerContorller);
	void SetupInventroyAndEquipment(APlayerController* PlayerContorller);

	UInventory* GetInventory() const { return Inventory; }
	UInventory* GetGemSlots() const { return GemSlots; }

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

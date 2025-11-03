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
class APlayerCharacterController;

UCLASS()
class SPARTA_TPROJECT_02_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Inventory")
	UInventoryWidget* InventoryWidgetInstance;

	virtual void Init() override;
	
public:
	UMyGameInstance();

	UInventoryWidget* GetInventoryWidget() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInventory* Inventory;

	void SetupInventoryWidget(APlayerCharacterController* PlayerContorller);

};

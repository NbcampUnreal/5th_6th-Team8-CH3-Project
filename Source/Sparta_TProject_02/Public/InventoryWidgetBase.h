#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidgetBase.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API UInventoryWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool RefreshWidget();
};

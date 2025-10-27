#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "MaterialItem.generated.h"

UCLASS()
class UMaterialItem : public UItem
{
	GENERATED_BODY()

protected:
	int32 CurrentStack;
	int32 MaxStack;
public:
	UMaterialItem();
	virtual int32 GetCurrentStack();
	virtual int32 GetItemMaxStack();
};

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item.generated.h"

class APickupItem;

UCLASS(Blueprintable, BlueprintType)
class UItem : public UDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftClassPtr<APickupItem> PickupItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<UTexture2D> ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 ItemPrice;

	bool bIsStackable;

public:
	UItem();
	virtual FName GetItemName();
	virtual FName GetItemType();
	virtual int32 GetItemPrice();
	virtual bool IsStackable();
	virtual void Use();
};

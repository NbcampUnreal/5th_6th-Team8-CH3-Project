#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemButtonWidget.generated.h"

class UButton;

USTRUCT(BlueprintType)
struct FItemButtonData
{
	GENERATED_BODY()

	int32 Index;
	FVector2D ButtonPosition;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredSignature, const FItemButtonData&, ItemButtonData);

UCLASS()
class SPARTA_TPROJECT_02_API UItemButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void HandleButtonHovered();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
	UButton* Button;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
	FItemButtonData ButtonData;

	virtual void NativeConstruct() override;
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHoveredSignature OnHovered;

	UButton* GetButton() const;
	FItemButtonData GetButtonData() const;
	void SetButton(UButton* NewButton);
	void SetButtonData(FItemButtonData NewButtonData);
	UFUNCTION()
	void ButtonBinding();
};

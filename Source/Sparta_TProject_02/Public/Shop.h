#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory.h"
#include "Item.h"
#include "ShopItemData.h"
#include "Shop.generated.h"

class UShopUserWidget;

UCLASS()
class SPARTA_TPROJECT_02_API AShop : public AActor
{
	GENERATED_BODY()

public:
	AShop();
	void OpenShop();
	void CloseShop();
	bool IsShopVisible() const { return ShopVisible; }
	void SetShopPlayerController(APlayerController* InPC) { PC = InPC; }

	// 상점 UI 열기/닫기

	// 상점 목록 갱신
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UpdateList();

	// 항목 설명 갱신
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void UpdateDescription();

protected:
	virtual void BeginPlay() override;

	// 플레이어 컨트롤러
	UPROPERTY()
	APlayerController* PC;

	// 상점 UI 클래스 (WBP_ShopUI)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ShopWidgetClass;

	// 상점 위젯 인스턴스
	UPROPERTY()
	UUserWidget* ShopWidgetInstance;

	// 상점이 열려있는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	bool ShopVisible;

	// 판매 가능 아이템 목록
	UPROPERTY(EditAnywhere, Category = "Shop Data")
	TArray<FShopItemData> AvailableItems;
};

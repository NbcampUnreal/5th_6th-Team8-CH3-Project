#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UIManager.generated.h"

class UUserWidget;
class UGameHUDWidget;
class UShopWidget;

/**
 * 전역 UI 매니저
 *  - 여러 UI(전투 HUD, 상점, 결과창 등)를 중앙에서 관리
 *  - AddToViewport / RemoveFromParent 중복 방지
 *  - 상위 레이어 개념으로 HUD 위에 Shop, Pause UI 표시 가능
 */
UCLASS()
class SPARTA_TPROJECT_02_API UUIManager : public UObject
{
    GENERATED_BODY()

public:
    // 초기화 (PlayerController로부터 호출)
    void Init(APlayerController* InPC);

    // --- UI 표시 함수들 ---
    void ShowHUD();
    void ShowShop();
    void HideShop();
    void ShowGameOver();
    void ClearAllUI();

protected:
    // --- 내부 관리용 ---
    UPROPERTY()
    APlayerController* PC;

    UPROPERTY()
    UGameHUDWidget* GameHUD;

    UPROPERTY()
    UUserWidget* CurrentPopup; // Shop, Result 등 임시 UI 저장

    // --- UI 클래스 레퍼런스 (블루프린트에서 지정 가능) ---
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGameHUDWidget> GameHUDClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> ShopWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;
};

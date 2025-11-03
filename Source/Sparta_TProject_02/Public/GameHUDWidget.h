#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUDWidget.generated.h"


UCLASS()
class SPARTA_TPROJECT_02_API UGameHUDWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    // 위젯이 생성될 때(Construct) 호출되는 함수
    virtual void NativeConstruct() override;

    // --- [델리게이트 수신용 함수들] ---
    UFUNCTION()
    void OnHealthChanged(int32 NewHP, int32 MaxHP);

    UFUNCTION()
    void OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);

    UFUNCTION()
    void OnScoreChanged(int32 NewScore);

    UFUNCTION()
    void OnWaveChanged(int32 NewWave);

    UFUNCTION()
    void OnEnemyHit();

    // --- [BP에서 구현할 비주얼 이벤트들] ---
    // 디자이너가 블루프린트에서 시각적 처리를 구현함
    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void UpdateHealthVisual(float Ratio);

    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void UpdateAmmoVisual(int32 CurrentAmmo, int32 MaxAmmo);

    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void UpdateScoreVisual(int32 Score);

    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void UpdateWaveVisual(int32 WaveNum);

    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void PlayHitMarkerVisual();
};

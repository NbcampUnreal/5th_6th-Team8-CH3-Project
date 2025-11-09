#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUDWidget.generated.h"


UCLASS()
class SPARTA_TPROJECT_02_API UGameHUDWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void BindToDelegates();

    UFUNCTION()
    void OnHealthChanged(float NewHP, float MaxHP);

    UFUNCTION()
    void OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);

    UFUNCTION()
    void OnScoreChanged(int32 NewScore);

    UFUNCTION()
    void OnWaveChanged(int32 NewWave);

    UFUNCTION()
    void OnEnemyHit();

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

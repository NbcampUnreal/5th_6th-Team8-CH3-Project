#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUDWidget.generated.h"

class APlayerCharacter;
class UTextBlock;
class UProgressBar;

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

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    
    APlayerController* PlayerController;
    //APlayerCharacter* PC;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")
    UProgressBar* HP_Bar;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")

    UTextBlock* Weapon_Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")

    UTextBlock* Ammo_Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")

    UTextBlock* Wave_Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")

    UTextBlock* Kill_Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")

    UTextBlock* Coin_Text;
};


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUDWidget.generated.h"


UCLASS()
class SPARTA_TPROJECT_02_API UGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void SetHealth(float NewHP);

    UFUNCTION(BlueprintCallable)
    void SetWaveInfo(int32 CurrentWave, float TimeToNextWave);

private:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar = nullptr;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WaveText = nullptr;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* NextWaveText = nullptr;
};

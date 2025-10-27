#include "GameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGameHUDWidget::SetHealth(float NewHP)
{
    if (HealthBar)
        HealthBar->SetPercent(NewHP);
}

void UGameHUDWidget::SetWaveInfo(int32 CurrentWave, float TimeToNextWave)
{
    if (WaveText)
        WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave)));

    if (NextWaveText)
        NextWaveText->SetText(FText::FromString(FString::Printf(TEXT("Next Wave: %.1f s"), TimeToNextWave)));
}

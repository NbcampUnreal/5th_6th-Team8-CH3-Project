#include "GameHUDWidget.h"
#include "STGameState.h"
#include "PlayerCharacter.h"

void UGameHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BindToDelegates();  // 연결 부분을 함수로 분리해 호출
}

void UGameHUDWidget::BindToDelegates()
{
    if (UWorld* World = GetWorld())
    {
        // --- GameState 델리게이트 연결 ---
        if (ASTGameState* GS = World->GetGameState<ASTGameState>())
        {
            if (!GS->OnCurrentScoreChanged.IsAlreadyBound(this, &UGameHUDWidget::OnScoreChanged))
            {
                GS->OnCurrentScoreChanged.AddDynamic(this, &UGameHUDWidget::OnScoreChanged);
            }

            if (!GS->OnCurrentWaveChanged.IsAlreadyBound(this, &UGameHUDWidget::OnWaveChanged))
            {
                GS->OnCurrentWaveChanged.AddDynamic(this, &UGameHUDWidget::OnWaveChanged);
            }
        }

        // --- PlayerCharacter 델리게이트 연결 ---
        if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
        {
            if (!PC->OnHealthChanged.IsAlreadyBound(this, &UGameHUDWidget::OnHealthChanged))
            {
                PC->OnHealthChanged.AddDynamic(this, &UGameHUDWidget::OnHealthChanged);
            }

            if (!PC->OnAmmoChanged.IsAlreadyBound(this, &UGameHUDWidget::OnAmmoChanged))
            {
                PC->OnAmmoChanged.AddDynamic(this, &UGameHUDWidget::OnAmmoChanged);
            }

            if (!PC->OnEnemyHit.IsAlreadyBound(this, &UGameHUDWidget::OnEnemyHit))
            {
                PC->OnEnemyHit.AddDynamic(this, &UGameHUDWidget::OnEnemyHit);
            }
        }
    }
}

// --- 델리게이트 콜백들 ---
void UGameHUDWidget::OnHealthChanged(float NewHP, float MaxHP)
{
    const float Ratio = (MaxHP > KINDA_SMALL_NUMBER) ? (NewHP / MaxHP) : 0.0f;
    UpdateHealthVisual(FMath::Clamp(Ratio, 0.f, 1.f));
}

void UGameHUDWidget::OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
    UpdateAmmoVisual(CurrentAmmo, MaxAmmo);
}

void UGameHUDWidget::OnScoreChanged(int32 NewScore)
{
    UpdateScoreVisual(NewScore);
}

void UGameHUDWidget::OnWaveChanged(int32 NewWave)
{
    UpdateWaveVisual(NewWave);
}

void UGameHUDWidget::OnEnemyHit()
{
    PlayHitMarkerVisual();
}

#include "GameHUDWidget.h"
//#include "STGameState.h"
#include "PlayerCharacter.h"

void UGameHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    /* --- GameState 델리게이트 구독 ---
    if (ASTGameState* GS = GetWorld()->GetGameState<ASTGameState>())
    {
        // 점수/웨이브 델리게이트 등록
        GS->OnScoreChanged.AddDynamic(this, &UGameHUDWidget::OnScoreChanged);
        GS->OnCurrentWaveChanged.AddDynamic(this, &UGameHUDWidget::OnWaveChanged);
    }

    // --- PlayerCharacter 델리게이트 구독 ---
    if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        // 체력, 탄약, 피격 이벤트 델리게이트 등록
        PC->OnHealthChanged.AddDynamic(this, &UGameHUDWidget::OnHealthChanged);
        PC->OnAmmoChanged.AddDynamic(this, &UGameHUDWidget::OnAmmoChanged);
        PC->OnEnemyHit.AddDynamic(this, &UGameHUDWidget::OnEnemyHit);
    }
    */
}

// 체력 변경 시 호출됨 → BP에서 ProgressBar, 색상 등 갱신
void UGameHUDWidget::OnHealthChanged(int32 NewHP, int32 MaxHP)
{
    float Ratio = FMath::Clamp((float)NewHP / MaxHP, 0.f, 1.f);
    UpdateHealthVisual(Ratio); // BP 이벤트 호출
}

// 탄약 변경 시 호출됨 → BP에서 텍스트 변경 처리
void UGameHUDWidget::OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
    UpdateAmmoVisual(CurrentAmmo, MaxAmmo); // BP 이벤트 호출
}

// 점수 변경 시 호출됨 → BP에서 점수 텍스트 갱신
void UGameHUDWidget::OnScoreChanged(int32 NewScore)
{
    UpdateScoreVisual(NewScore); // BP 이벤트 호출
}

// 웨이브 변경 시 호출됨 → BP에서 “Wave : n” 표시
void UGameHUDWidget::OnWaveChanged(int32 NewWave)
{
    UpdateWaveVisual(NewWave); // BP 이벤트 호출
}

// 적 피격 시 호출됨 → BP에서 히트마커 연출 실행
void UGameHUDWidget::OnEnemyHit()
{
    PlayHitMarkerVisual(); // BP 이벤트 호출
}

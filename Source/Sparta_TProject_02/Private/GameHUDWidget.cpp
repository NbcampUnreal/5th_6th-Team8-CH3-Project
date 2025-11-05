#include "GameHUDWidget.h"
#include "STGameState.h"
#include "PlayerCharacter.h"

void UGameHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ASTGameState* GS = GetWorld()->GetGameState<ASTGameState>())
    {
        // ����/���̺� ��������Ʈ ���
        GS->OnCurrentScoreChanged.AddDynamic(this, &UGameHUDWidget::OnScoreChanged);
        GS->OnCurrentWaveChanged.AddDynamic(this, &UGameHUDWidget::OnWaveChanged);
    }

    // --- PlayerCharacter ��������Ʈ ���� ---
    if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        // ü��, ź��, �ǰ� �̺�Ʈ ��������Ʈ ���
        PC->OnHealthChanged.AddDynamic(this, &UGameHUDWidget::OnHealthChanged);
        PC->OnAmmoChanged.AddDynamic(this, &UGameHUDWidget::OnAmmoChanged);
        PC->OnEnemyHit.AddDynamic(this, &UGameHUDWidget::OnEnemyHit);
    }
    
}

// ü�� ���� �� ȣ��� �� BP���� ProgressBar, ���� �� ����
void UGameHUDWidget::OnHealthChanged(float NewHP, float MaxHP)
{
    float Ratio = FMath::Clamp((float)NewHP / MaxHP, 0.f, 1.f);
    UpdateHealthVisual(Ratio); // BP �̺�Ʈ ȣ��
}

// ź�� ���� �� ȣ��� �� BP���� �ؽ�Ʈ ���� ó��
void UGameHUDWidget::OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
    UpdateAmmoVisual(CurrentAmmo, MaxAmmo); // BP �̺�Ʈ ȣ��
}

// ���� ���� �� ȣ��� �� BP���� ���� �ؽ�Ʈ ����
void UGameHUDWidget::OnScoreChanged(int32 NewScore)
{
    UpdateScoreVisual(NewScore); // BP �̺�Ʈ ȣ��
}

// ���̺� ���� �� ȣ��� �� BP���� ��Wave : n�� ǥ��
void UGameHUDWidget::OnWaveChanged(int32 NewWave)
{
    UpdateWaveVisual(NewWave); // BP �̺�Ʈ ȣ��
}

// �� �ǰ� �� ȣ��� �� BP���� ��Ʈ��Ŀ ���� ����
void UGameHUDWidget::OnEnemyHit()
{
    PlayHitMarkerVisual(); // BP �̺�Ʈ ȣ��
}

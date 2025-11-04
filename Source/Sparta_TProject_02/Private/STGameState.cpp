#include "STGameState.h"
#include "STGameMode.h"

ASTGameState::ASTGameState()
{
	// 기본값 설정
	CurrentWave = 0;
	EnemiesRemaining = 0;
	TotalEnemiesInWave = 0;
	CurrentScore = 0;
	WavesCleared = 0;
	TotalPlayTime = 0.0f;
	bIsBossWave = false;
	bIsGameOver = false;
	bIsGameWin = false;
}

// --- GameMode가 호출하는 Set 함수
//-UI
void ASTGameState::SetCurrentWave(int32 NewWave)
{
	if (CurrentWave == NewWave) return;
	CurrentWave = NewWave;
	OnCurrentWaveChanged.Broadcast(CurrentWave);
}

void ASTGameState::SetEnemiesRemaining(int32 NewCount)
{
	if (EnemiesRemaining == NewCount) return;
	EnemiesRemaining = NewCount;
	OnEnemiesRemainingChanged.Broadcast(EnemiesRemaining);
}

void ASTGameState::SetTotalEnemiesInWave(int32 NewTotal)
{
	if (TotalEnemiesInWave == NewTotal) return;
	TotalEnemiesInWave = NewTotal;
	OnTotalEnemiesInWaveChanged.Broadcast(TotalEnemiesInWave);
}

void ASTGameState::SetScore(int32 NewScore)
{
	if (CurrentScore == NewScore) return;
	CurrentScore = NewScore;
	OnCurrentScoreChanged.Broadcast(CurrentScore);
}

void ASTGameState::AddScore(int32 ScoreDelta)
{
	if (ScoreDelta == 0) return;
	CurrentScore += ScoreDelta;
	OnCurrentScoreChanged.Broadcast(CurrentScore);
}

//-GameState

void ASTGameState::SetIsBossWave(bool bIsBoss)
{
	if (bIsBossWave == bIsBoss) return;
	bIsBossWave = bIsBoss;
	OnIsBossWaveChanged.Broadcast(bIsBossWave);
}

void ASTGameState::SetTimeRemaining(int32 NewTime)
{
	if (TimeRemaining == NewTime) return;
	TimeRemaining = NewTime;
	OnTimeRemainingChanged.Broadcast(TimeRemaining);

}

void ASTGameState::SetGameOver(bool bISGameOver)
{
	if (bIsGameOver == bISGameOver) return;
	bIsGameOver = bISGameOver;
	OnIsGameOverChanged.Broadcast(bIsGameOver);
}

void ASTGameState::SetGameWin(bool bISWin)
{
	if (bIsGameWin == bISWin) return;
	bIsGameWin = bISWin;
	OnIsGameWinChanged.Broadcast(bIsGameWin);
}

int32 ASTGameState::GetCurrentScore() const
{
	return CurrentScore;
}

void ASTGameState::OnWaveCleared()
{
	WavesCleared++;
	OnWavesClearedChanged.Broadcast(WavesCleared);
}
#include "STGameState.h"
#include "Net/UnrealNetwork.h"

ASTGameState::ASTGameState()
{
	CurrentWave = 0;
	EnemiesRemaining = 0;
	TotalEnemiesInWave = 0;
	CurrentScore = 0;
	WavesCleared = 0;
	bIsBossWave = false;
	bIsGameOver = false;
	bIsGameWin = false;
}

//네트워크 복제
void ASTGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASTGameState, CurrentWave);
	DOREPLIFETIME(ASTGameState, EnemiesRemaining);
	DOREPLIFETIME(ASTGameState, TotalEnemiesInWave);
	DOREPLIFETIME(ASTGameState, CurrentScore);
	DOREPLIFETIME(ASTGameState, WavesCleared);
	DOREPLIFETIME(ASTGameState, bIsBossWave);
	DOREPLIFETIME(ASTGameState, bIsGameOver);
	DOREPLIFETIME(ASTGameState, bIsGameWin);
}

//setter
void ASTGameState::SetCurrentWave(int32 NewWave)
{
	CurrentWave = NewWave;
}

void ASTGameState::SetEnemiesRemaining(int32 NewCount)
{
	EnemiesRemaining = NewCount;
	if (HasAuthority())
	{
		OnRep_EnemiesRemaining();
	}
}

void ASTGameState::SetTotalEnemiesInWave(int32 NewTotal)
{
	TotalEnemiesInWave = NewTotal;
}

void ASTGameState::SetScore(int32 NewScore)
{
	CurrentScore = NewScore;

	if (HasAuthority())
	{
		OnRep_CurrentScore();
	}
}

void ASTGameState::SetIsBossWave(bool bIsBoss)
{
	bIsBossWave = bIsBoss;
}

void ASTGameState::SetGameOver(bool bIsGameOver)
{
	bIsGameOver = bIsGameOver;
}

void ASTGameState::SetGameWin(bool bIsGameWin)
{
	bIsGameWin = bIsGameWin;
}

void ASTGameState::OnWaveCleared()
{
	WavesCleared++;
}

//콜백 UI 업뎃

void ASTGameState::OnRep_CurrentWave()
{
	UE_LOG(LogTemp, Log, TEXT("[GameState] Updated Wave... : Wave %d"), CurrentWave);

}

void ASTGameState::OnRep_EnemiesRemaining()
{
	UE_LOG(LogTemp, Log, TEXT("[GameState] Remained Enemies: %d / %d"), EnemiesRemaining, TotalEnemiesInWave);
}

void ASTGameState::OnRep_CurrentScore()
{
	UE_LOG(LogTemp, Log, TEXT("[GameState] Updated Score: %d"), CurrentScore);
}


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "STGameState.generated.h" 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoolChangedSignature, bool, bNewValue);

UCLASS()
class SPARTA_TPROJECT_02_API ASTGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASTGameState();

	// --- GameMode�� ȣ���� �Լ��� ---
	void SetCurrentWave(int32 NewWave);
	void SetEnemiesRemaining(int32 NewCount);
	void SetTotalEnemiesInWave(int32 NewTotal);
	void SetScore(int32 NewScore);
	void AddScore(int32 ScoreDelta);
	void SetIsBossWave(bool bIsBoss);
	void SetGameOver(bool bISGameOver);
	void SetGameWin(bool bISWin);
	void OnWaveCleared();

	int32 GetCurrentScore() const;

	void SetTimeRemaining(int32 NewTime);

	// --- UI���� ���ε�(����)�� �̺�Ʈ ---
	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnIntChangedSignature OnCurrentWaveChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnIntChangedSignature OnTimeRemainingChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnIntChangedSignature OnEnemiesRemainingChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnIntChangedSignature OnTotalEnemiesInWaveChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnIntChangedSignature OnCurrentScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnIntChangedSignature OnWavesClearedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnBoolChangedSignature OnIsBossWaveChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnBoolChangedSignature OnIsGameOverChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnBoolChangedSignature OnIsGameWinChanged;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnIntChangedSignature OnKillCountChanged;

	// --- ���� ���� ������ ---
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	int32 CurrentWave;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	int32 EnemiesRemaining;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	int32 TotalEnemiesInWave;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	int32 CurrentScore;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	float TotalPlayTime;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	int32 TimeRemaining;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	int32 WavesCleared;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	bool bIsBossWave;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	bool bIsGameOver;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game State")
	bool bIsGameWin;


};

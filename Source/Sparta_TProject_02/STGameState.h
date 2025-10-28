#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "STGameState.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API ASTGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ASTGameState();

	//getter

	UFUNCTION(BlueprintCallable, Category = "Game State")
	int32 GetCurrentWave() const { return CurrentWave; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	int32 GetEnemiesRemaining() const { return EnemiesRemaining; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	int32 GetTotalEnemiesInWave() const { return TotalEnemiesInWave; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	int32 GetCurrentScore() const { return CurrentScore; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	int32 GetWavesCleared() const { return WavesCleared; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	bool IsBossWave() const { return bIsBossWave; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	bool IsGameOver() const { return bIsGameOver; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	bool IsGameWin() const { return bIsGameWin; }

	//setter

	void SetCurrentWave(int32 NewWave);
	void SetEnemiesRemaining(int32 NewCount);
	void SetTotalEnemiesInWave(int32 NewTotal);
	void SetScore(int32 NewScore);
	void SetIsBossWave(bool bIsBoss);
	void SetGameOver(bool bISGameOver);
	void SetGameWin(bool bISWin);

	void OnWaveCleared();

protected:
	//복제 변수 UI가 읽을 상태
	 
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWave, BlueprintReadOnly, Category = "Game State")
	int32 CurrentWave;


	UPROPERTY(ReplicatedUsing = OnRep_EnemiesRemaining, BlueprintReadOnly, Category = "Game State")
	int32 EnemiesRemaining;


	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	int32 TotalEnemiesInWave;


	UPROPERTY(ReplicatedUsing = OnRep_CurrentScore, BlueprintReadOnly, Category = "Game State")
	int32 CurrentScore;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	int32 WavesCleared;

//----UI 표시용
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bIsBossWave;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bIsGameOver;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bIsGameWin;

	//  복제 콜백 함수
	UFUNCTION()
	void OnRep_CurrentWave();

	UFUNCTION()
	void OnRep_EnemiesRemaining();

	UFUNCTION()
	void OnRep_CurrentScore();

	//네트워크 복제 설정

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TimerManager.h"
#include "STGameMode.generated.h"

class ASTSpawnVolume;
class ASTGameState;
class AAIMonsterBase;
class APlayerCharacter;
class APlayerCharacterController;

UCLASS()
class SPARTA_TPROJECT_02_API ASTGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASTGameMode();

	// [EnemyCharacter] 캐릭터가 죽었을 때 호출
	void OnEnemyKilled(AAIMonsterBase* Die);

	// 웨이브 증가에 따른 적 체력 증가
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float HealthIncreaseMultiplier = 1.15f;

	float GetHealthMultiplierForWave(int32 Wave) const;

protected:
	// 게임 시작 호출
	virtual void BeginPlay() override;

	// [PlayerCharacter] 플레이어 사망 시 호출
	virtual void PawnKilled(APawn* PawnKilled);

	void OnPlayerDeath(APlayerCharacter* DeadPlayer);

	// 다음 웨이브 시작
	void StartNextWave();

	void KillAllRemainingEnemies();

	// 현재 웨이브에 따라 스폰할 적의 총 수 계산
	int32 CalculateEnemiesForWave(int32 Wave) const;

	// 현재 웨이브에 맞게 적 생성
	void SpawnEnemies();

	void SpawnBoss(TSubclassOf<AAIMonsterBase> BossClassToSpawn);

	// 웨이브 클리어 조건에 호출
	void OnWaveCleared();

	// 웨이브 대기 시간 종료 후 호출
	void HandleWaveDelay();

	void HandleGameOver();

	void HandleGameWin();

	// 웨이브 클리어 조건 체크
	void CheckWaveClearCondition();

	// 웨이브별 필요 킬 수 반환
	int32 GetRequiredKillsForWave(int32 Wave);

	// 타임리밋 초과 시 호출
	void OnWaveTimeLimit();

	// 스폰할 위치를 랜덤 위치값으로 가져옴
	UPROPERTY(VisibleInstanceOnly, Category = "GameMode")
	TArray<ASTSpawnVolume*> SpawnVolumes;

	//-------함수-------
	bool GetRandomSpawnLocation(FVector& OutSpawnLocation);
	void AddScore(int32 ScoreToAdd);
	void HealPlayer(float HealAmount);

protected:

	//---에디터에서 설정 필요
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Spawning")
	TSubclassOf<AAIMonsterBase> EnemyClass;

	//---- 좀비 추가
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Spawning")
	TSubclassOf<AAIMonsterBase> SecondEnemyClass;

	//---- 보스 추가
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Spawning")
	TSubclassOf<AAIMonsterBase> FinalEnemyClass;

	// 첫 웨이브에 나오는 기본 적의 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 MonsterPerWave = 50;

	// 적의 증가
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	int32 MonsterIncreasePerWave = 50;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "2"))
	int32 ZombiePerWave = 10;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "2"))
	int32 ZombieStartWave = 2;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "3"))
	int32 FinalBossWave = 3;

	//  웨이브별 클리어 조건
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave Clear",
		meta = (ClampMin = "1", ToolTip = "좀비(첫 번째 적) 웨이브 클리어에 필요한 킬 수"))
	int32 ZombieKillsRequiredForClear = 15;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave Clear",
		meta = (ClampMin = "1", ToolTip = "Rampage(두 번째 적) 웨이브 클리어에 필요한 킬 수"))
	int32 RampageKillsRequiredForClear = 20;

	//웨이브 타임리밋 (5분)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave Clear",
		meta = (ClampMin = "60.0", ToolTip = "웨이브 최대 시간 (초) - 초과 시 자동으로 다음 웨이브"))
	float MaxWaveTimeLimit = 300.0f;

	// 웨이브 클리어 시 회복량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave",
		meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float HealAmountOnWaveClear = 25.0f;

	// 웨이브 시작 대기 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float DelayBetweenWaves = 5.f;

	// 적 처치 시 획득할 기본 점수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Scoring", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 ScorePerKill = 100;

	// 보스 처치 시 획득 점수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Scoring", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 BossKillScore = 500;

	// 웨이브 클리어 시 보너스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Scoring", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	int32 WaveClearBonus = 500;

	UPROPERTY()
	TObjectPtr<ASTGameState> STGameState;

protected:
	// 현재 웨이브 번호
	int32 CurrentWave = 0;

	// 현재 필드에 살아있는 적의 수
	int EnemiesAlive = 0;

	int32 TotalEnemiesSpawned = 0;

	//  현재 웨이브에서 죽인 적의 수
	int32 CurrentWaveKills = 0;

	//현재 웨이브 클리어에 필요한 킬 수
	int32 RequiredKillsForCurrentWave = 0;

	// 다음 웨이브 딜레이 타이머 핸들
	FTimerHandle TimerHandleWaveDelay;

	// 웨이브 타임리밋 타이머 핸들
	FTimerHandle WaveTimeLimitTimer;

	bool bIsGameOver = false;

	bool bIsBossWave = false;
};
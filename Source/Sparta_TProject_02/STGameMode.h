#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TimerManager.h"
#include "STGameMode.generated.h"

class STSpawnVolume;
class ASTGameState;
class AAIMonsterBase;
class APlayerCharacter;
class APlayerCharacterController;

UCLASS()
class SPARTA_TPROJECT_02_API ASTGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASTGameMode();

	//[EnemyCharacter]적 캐릭터가 죽었을 때 호출
	void OnEnemyKilled(AAIMonsterBase* Die);

	//웨이브 증가에 따른 적 체력 증가
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float HealthIncreaseMultiplier = 1.15f;

	float GetHealthMultiplierForWave(int32 Wave) const;
	

protected:
	//게임 시작 호출
	virtual void BeginPlay() override;

	//[PlayerCharacter] 플레이어 사망 시 호출
	//virtual void PawnKilled(APawn* PawnKilled);

	void OnPlayerDeath(APlayerCharacter* DeadPlayer);

	//다음 웨이브 시작
	void StartNextWave();

	//현재 웨이브에 따른 스폰할 적의 개수 계산
	int32 CalculateEnemiesForWave(int32 Wave) const;

	//현재 웨이브에 맞는 적 스폰
	void SpawnEnemies();

	void SpawnBoss(TSubclassOf<AAIMonsterBase> BossClassToSpawn);

	//웨이브 클리어 성공시 호출
	void OnWaveCleared();

	//웨이브 대기 시간 끝났을 때 호출
	void HandleWaveDelay();

	void HandleGameOver();

	void HandleGameWin();

	//-------헬퍼-------
	FVector GetRandomSpawnLocation() const;
	void AddScore(int32 ScoreToAdd);
	void HealPlayer(float HealAmount);

protected:
	//----------------------웨이브 설정 (블루프린트)------------------------

	//---에디터에서 몬스터 선택
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Spawning")
	TSubclassOf<AAIMonsterBase> EnemyClass;	

	//----좀비 선택
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Spawning")
	TSubclassOf<AAIMonsterBase> SecondEnemyClass;

	//----보스 선택
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Spawning")
	TSubclassOf<AAIMonsterBase> FinalEnemyClass;

	//첫 웨이브에 스폰할 기본 적의 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 MonsterPerWave = 50;

	//몬스터 증가
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	int32 MonsterIncreasePerWave = 50;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "2"))
	int32 ZombiePerWave = 10;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "2"))
	int32 ZombieStartWave = 2;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "3"))
	int32 FinalBossWave = 3;

	//웨이브 클리어 시 회복량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave",
		meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float HealAmountOnWaveClear = 25.0f;



	//웨이브 사이의 대기 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float DelayBetweenWaves = 5.f;

	//적 처치 시 획득할 기본 점수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Scoring", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 ScorePerKill = 100;

	//보스 처치 시 획득 점수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Scoring", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 BossKillScore = 500;

	//웨이브 클리어 시 보너스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Scoring", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	int32 WaveClearBonus = 500;

	//레벨에 배치된 스폰 볼륨들의 목록
	UPROPERTY()
	TArray<TObjectPtr<STSpawnVolume>> SpawnVolumes;

	UPROPERTY()
	TObjectPtr<ASTGameState> STGameState;

	UPROPERTY()
	TObjectPtr<ASTGameState> STGameState;

protected:
	//현재 웨이브 번호
	int32 CurrentWave = 0;

	//현재 맵에 살아있는 적의 수
	int EnemiesAlive = 0;;

	int32 TotalEnemiesSpawned = 0;

	//다음 웨이브 시작을 위한 타이머 핸들
	FTimerHandle TimerHandleWaveDelay;

	bool bIsGameOver = false;

	bool bIsBossWave = false;
	
};

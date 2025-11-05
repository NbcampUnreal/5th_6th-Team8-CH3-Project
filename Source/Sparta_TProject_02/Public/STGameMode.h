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

	//[EnemyCharacter]�� ĳ���Ͱ� �׾��� �� ȣ��
	void OnEnemyKilled(AAIMonsterBase* Die);

	//���̺� ������ ���� �� ü�� ����
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float HealthIncreaseMultiplier = 1.15f;

	float GetHealthMultiplierForWave(int32 Wave) const;

protected:
	//���� ���� ȣ��
	virtual void BeginPlay() override;

	//[PlayerCharacter] �÷��̾� ��� �� ȣ��
	virtual void PawnKilled(APawn* PawnKilled);

	void OnPlayerDeath(APlayerCharacter* DeadPlayer);

	//���� ���̺� ����
	void StartNextWave();

	//���� ���̺꿡 ���� ������ ���� ���� ���
	int32 CalculateEnemiesForWave(int32 Wave) const;

	//���� ���̺꿡 �´� �� ����
	void SpawnEnemies();

	void SpawnBoss(TSubclassOf<AAIMonsterBase> BossClassToSpawn);

	//���̺� Ŭ���� ������ ȣ��
	void OnWaveCleared();

	//���̺� ��� �ð� ������ �� ȣ��
	void HandleWaveDelay();

	void HandleGameOver();

	void HandleGameWin();

	//������ ��ġ�� ���� �������� ���

	UPROPERTY(VisibleInstanceOnly, Category = "GameMode")
	TArray<ASTSpawnVolume*> SpawnVolumes;

	//-------����-------
	bool GetRandomSpawnLocation(FVector& OutSpawnLocation);
	void AddScore(int32 ScoreToAdd);
	void HealPlayer(float HealAmount);

protected:

	//---�����Ϳ��� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Spawning")
	TSubclassOf<AAIMonsterBase> EnemyClass;

	//----���� ����
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Spawning")
	TSubclassOf<AAIMonsterBase> SecondEnemyClass;

	//----���� ����
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Spawning")
	TSubclassOf<AAIMonsterBase> FinalEnemyClass;

	//ù ���̺꿡 ������ �⺻ ���� ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 MonsterPerWave = 50;

	//���� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	int32 MonsterIncreasePerWave = 50;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "2"))
	int32 ZombiePerWave = 10;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "2"))
	int32 ZombieStartWave = 2;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "3"))
	int32 FinalBossWave = 3;

	//���̺� Ŭ���� �� ȸ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave",
		meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float HealAmountOnWaveClear = 25.0f;



	//���̺� ������ ��� �ð�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Wave", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float DelayBetweenWaves = 5.f;

	//�� óġ �� ȹ���� �⺻ ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Scoring", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 ScorePerKill = 100;

	//���� óġ �� ȹ�� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Scoring", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 BossKillScore = 500;

	//���̺� Ŭ���� �� ���ʽ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Scoring", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	int32 WaveClearBonus = 500;

	UPROPERTY()
	TObjectPtr<ASTGameState> STGameState;

protected:
	//���� ���̺� ��ȣ
	int32 CurrentWave = 0;

	//���� �ʿ� ����ִ� ���� ��
	int EnemiesAlive = 0;;

	int32 TotalEnemiesSpawned = 0;

	//���� ���̺� ������ ���� Ÿ�̸� �ڵ�
	FTimerHandle TimerHandleWaveDelay;

	bool bIsGameOver = false;

	bool bIsBossWave = false;


	};



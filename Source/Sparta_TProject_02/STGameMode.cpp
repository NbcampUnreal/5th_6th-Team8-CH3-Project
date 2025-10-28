#include "STGameMode.h"
#include "STGameState.h"
#include "STSpawnVolume.h"
#include "AIMonsterBase.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

ASTGameMode::ASTGameMode()
{
	GameStateClass = ASTGameState::StaticClass();

	CurrentWave = 0;
	EnemiesAlive = 0;
	TotalEnemiesSpawned = 0;
	bIsGameOver = false;
	bIsBossWave = false;
}

void ASTGameMode::BeginPlay()
{
	Super::BeginPlay();

	STGameState = GetGameState<ASTGameState>(GameState);
	if (!STGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("STGameState is null!"));
		return;
	}

	for (TActorIterator<ASTSpawnVolume> It(GetWorld()); It; ++It)
	{
		SpawnVolumes.Add(*It);
	}
	if (SpawnVolumes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] SpawnVolume Failed !"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[GameMode] FoundOut SpawnVolume %d !"), SpawnVolumes.Num());
	}

	//첫 웨이브 시작 3초 후
	FTimerHandle InitialDelayTimer;
	GetWorldTimerManager().SetTimer(
		InitialDelayTimer, 
		this,
		&ASTGameMode::StartNextWave, 
		3.f, 
		false
	);
	UE_LOG(LogTemp, Log, TEXT("[GameMode]START GAME!"));
	
}

void ASTGameMode::OnPlayerDeath(APlayerCharacter* DeadPlayer)
{
	if (!DeadPlayer || bIsGameOver)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[GameMode] DEAD, GAME OVER..."));
	HandleGameOver();
}

void ASTGameMode::StartNextWave()
{
	if (bIsGameOver)
	{
		return;
	}
	//웨이브 +
	CurrentWave++;
	//보스 웨이브인지
	bIsBossWave = (CurrentWave == FinalBossWave);

	UE_LOG(LogTemp, Log, TEXT("Start Wave !"), CurrentWave);

	if (STGameState)
	{
		STGameState->SetCurrentWave(CurrentWave);
		STGameState->SetIsBossWave(bIsBossWave);
	}
	//보스 웨이브일 경우
	if (bIsBossWave)
	{
		if (FinalEnemyClass)
		{
			SpawnBoss(FinalEnemyClass);
			UE_LOG(LogTemp, Warning, TEXT("[GameMode] Boss Entered !"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[GameMode] NOT SET FinalEnemyClass!"));
		}
	}
	else
	{
		SpawnEnemies();
	}
}

int32 ASTGameMode::CalculateEnemiesForWave(int32 Wave) const
{
	int32 BaseMonsters = MonsterPerWave;

	return BaseMonsters * (1 << (Wave -1));
}

//웨이브에 따른 체력 배율..흠...
float ASTGameMode::GetHealthMultiplierForWave(int32 Wave) const
{
	return FMath::Pow(HealthIncreaseMultiplier, Wave - 1);
}

// 적 스폰 로직
void ASTGameMode::SpawnEnemies()
{
	if (SpawnVolumes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] SpawnVolume이 없어서 적을 스폰할 수 없습니다!"));
		return;
	}

	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] EnemyClass가 설정되지 않았습니다!"));
		return;
	}

	// 스폰할 적 수 계산
	int32 EnemiesToSpawn = CalculateEnemiesForWave(CurrentWave);
	int32 ZombiesToSpawn = 0;

	// 좀비 등장 웨이브 이상이면 좀비 추가
	if (CurrentWave >= ZombieStartWave && SecondEnemyClass)
	{
		ZombiesToSpawn = ZombiePerWave;
		EnemiesToSpawn -= ZombiesToSpawn; // 일반 몬스터 수 조정
	}

	// 현재 웨이브의 체력 배율
	float HealthMultiplier = GetHealthMultiplierForWave(CurrentWave);

	// 일반 몬스터 스폰
	for (int32 i = 0; i < EnemiesToSpawn; i++)
	{
		FVector SpawnLocation = GetRandomSpawnLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AAIMonsterBase* SpawnedEnemy = GetWorld()->SpawnActor<AAIMonsterBase>(
			EnemyClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);

		if (SpawnedEnemy)
		{
			// 체력 배율 
			SpawnedEnemy->ApplyHealthMultiplier(HealthMultiplier);
			EnemiesAlive++;
			TotalEnemiesSpawned++;
		}
	}

	// 좀비 스폰
	for (int32 i = 0; i < ZombiesToSpawn; i++)
	{
		FVector SpawnLocation = GetRandomSpawnLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AAIMonsterBase* SpawnedZombie = GetWorld()->SpawnActor<AAIMonsterBase>(
			SecondEnemyClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);

		if (SpawnedZombie)
		{
			SpawnedZombie->ApplyHealthMultiplier(HealthMultiplier);
			EnemiesAlive++;
			TotalEnemiesSpawned++;
		}
	}

	// GameState 업데이트
	if (STGameState)
	{
		STGameState->SetEnemiesRemaining(EnemiesAlive);
		STGameState->SetTotalEnemiesInWave(TotalEnemiesSpawned);
	}

	UE_LOG(LogTemp, Log, TEXT("[GameMode] Wave %d: Finish %d Spawned (Monster: %d, Zombie: %d, Health: %.2f)"),
		CurrentWave, EnemiesAlive, EnemiesToSpawn, ZombiesToSpawn, HealthMultiplier);
}

void ASTGameMode::SpawnBoss(TSubclassOf<AAIMonsterBase> BossClassTOSpawn)
{
	if (SpawnVolumes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] NOT SPAWN VOLUMES"));
		return;
	}

	FVector SpawnLocation = GetRandomSpawnLocation();
	FRotator SpawnRotaition = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAIMonsterBase* SpawnedBoss = GetWorld()->SpawnActor<AAIMonsterBase>(
		BossClassToSpawn,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (SpawnedBoss)
	{
		float BossHealthMultiplier = GetHealthMultiplierForWave(CurrentWave) * 2.f;
		SpawnedBoss->ApplyHealthMultiplier(BossHealthMultiplier);

		EnemiesAlive = 1;
		TotalEnemiesSpawned = 1;

		if (STGameState)
		{
			STGameState->SetEnemiesRemaining(1);
			STGameState->SetTotalEnemiesInWave(1);
		}

		UE_LOG(LogTemp, Warning, TEXT("[GameMode] BOSS Spawned! Boos's Health: %.2f"), BossHealthMultiplier);
	}
}

// 랜덤 스폰 위치 선택
FVector ASTGameMode::GetRandomSpawnLocation() const
{
	if (SpawnVolumes.Num() == 0)
	{
		// 기본 위치 반환
		return FVector(0, 0, 100);
	}

	// 랜덤 SpawnVolume 선택
	int32 RandomIndex = FMath::RandRange(0, SpawnVolumes.Num() - 1);
	ASTSpawnVolume* SelectedVolume = SpawnVolumes[RandomIndex];

	if (SelectedVolume)
	{
		return SelectedVolume->GetRandomPointInVolume();
	}

	return FVector(0, 0, 100);
}

//[EnemyCharacter]적 캐릭터가 죽었을 때 호출
void ASTGameMode:: OnEnemyKilled(AAIMonsterBase* KilledEnemy)
{
	if (bIsGameOver) return;
	//살아있는 적 수 -1
	EnemiesAlive = FMath::Max(0, EnemiesAlive - 1);
	//점수 추가
	int32 ScoreToAdd = bIsBossWave ? BossKillScore : ScorePerKill;
	//GameState에 남은 적 수 업데이트

	if (STGameState)
	{
		STGameState->SetEnemiesRemaining(EnemiesAlive);
	}
	UE_LOG(LogGameMode, Log, TEXT("SUCCESSED KILL! remained enemies: %d"), EnemiesAlive, TotalEnemiesSpawned, ScoreToAdd);
	//남은 적이 0이면 웨이브 클리어 처리
	if (EnemiesAlive <= 0)
	{
		if (bIsBossWave)
		{
			HandleGameWin();
		}
		else {
			OnWaveCleared();
		}
	}
}

void ASTGameMode::OnWaveCleared()
{
	UE_LOG(LogGameMode, Log, TEXT("Cleared Wave %d !"), CurrentWave);

	AddScore(WaveClearBonus);
	HealPlayer(HealAmountOnWaveClear);

	if (STGameState)
	{
		STGameState->OnWaveCleared();
	}
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(PC);
		if (PlayerController)
		{
			PlayerController->TriggerStageReward();
		}
	}
	//다음 웨이브
	GetWorldTimerManager().SetTimer(
		TimerHandleWaveDelay,
		this,
		&ASTGameMode::HandleWaveDelay,
		DelayBetweenWaves,
		false
	);
	UE_LOG(LogTemp, Log, TEXT("[GameMode] START After %.1f Seconds"), DelayBetweenWaves);
}

void ASTGameMode::HandleWaveDelay()
{
	TotalEnemiesSpawned = 0;
	StartNextWave();
}

//게임 종료 후
void ASTGameMode::HandleGameOver()
{
	if (bIsGameOver)
	{
		return;
	}
	bIsGameOver = true;

	UE_LOG(LogTemp, Error, TEXT("GAME OVER !"));

	if (STGameState)
	{
		STGameState->SetGameOver(true);
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(PC);
		if (PlayerController)
		{
			PlayerController->OnGameOver();
		}
	}
}

void ASTGameMode::HandleGameWin()
{
	if (bIsGameOver)
	{
		return;
	}
	bIsGameOver = true;

	UE_LOG(LogTemp, Warning, TEXT("YOU WIN!"));

	// 승리 보너스 점수
	AddScore(BossKillScore * 2);

	if (STGameState)
	{
		STGameState->SetGameWin(true);
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(PC);
		if (PlayerController)
		{
			PlayerController->OnGameWin();
		}
	}
}

//-----헬퍼-------

void ASTGameMode::AddScore(int32 ScoreToAdd)
{
	if (STGameState)
	{
		int32 NewScore = STGameState->GetCurrentScore() + ScoreToAdd;
		STGameState->SetScore(NewScore);

		UE_LOG(LogTemp, Log, TEXT("[GameMode] ADD +%d (all score: %d)"), ScoreToAdd, NewScore);
	}
}

void ASTGameMode::HealPlayer(float HealAmount)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(PC->GetPawn());
		if (Player)
		{
			Player->HealOnWaveClear(HealAmount);
			UE_LOG(LogTemp, Log, TEXT("[GameMode] Healling... : % .1f HP"), HealAmount);
		}
	}
}




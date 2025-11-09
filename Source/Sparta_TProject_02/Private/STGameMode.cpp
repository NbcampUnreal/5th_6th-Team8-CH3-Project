#include "STGameMode.h"
#include "STGameState.h"
#include "STSpawnVolume.h"
#include "AIMonsterBase.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "STPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "NavigationSystem.h"

ASTGameMode::ASTGameMode()
{
	GameStateClass = ASTGameState::StaticClass();
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = APlayerCharacterController::StaticClass();

	CurrentWave = 0;
	EnemiesAlive = 0;
	TotalEnemiesSpawned = 0;
	bIsGameOver = false;
	bIsBossWave = false;

	// 새로운 변수 초기화
	CurrentWaveKills = 0;
	RequiredKillsForCurrentWave = 0;
}

void ASTGameMode::BeginPlay()
{
	Super::BeginPlay();

	STGameState = GetWorld()->GetGameState<ASTGameState>();
	if (!STGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("STGameState is null!"));
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTSpawnVolume::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (ASTSpawnVolume* SpawnVol = Cast<ASTSpawnVolume>(Actor))
		{
			SpawnVolumes.Add(SpawnVol);
		}
	}

	if (SpawnVolumes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] SpawnVolume Failed!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[GameMode] Found %d SpawnVolumes"), SpawnVolumes.Num());
	}

	FTimerHandle InitialDelayTimer;
	GetWorldTimerManager().SetTimer(
		InitialDelayTimer,
		this,
		&ASTGameMode::StartNextWave,
		3.f,
		false
	);
	UE_LOG(LogTemp, Log, TEXT("[GameMode] START GAME!"));
}

void ASTGameMode::PawnKilled(APawn* PawnKilled)
{
	if (bIsGameOver || !PawnKilled)
	{
		return;
	}

	APlayerCharacter* DeadPlayer = Cast<APlayerCharacter>(PawnKilled);
	if (DeadPlayer)
	{
		HandleGameOver();
	}
}

void ASTGameMode::OnPlayerDeath(APlayerCharacter* DeadPlayer)
{
	if (!DeadPlayer || bIsGameOver)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[GameMode] Player DEAD, GAME OVER..."));
	HandleGameOver();
}

void ASTGameMode::StartNextWave()
{
	if (bIsGameOver)
	{
		return;
	}

	// 웨이브 증가
	CurrentWave++;

	// 현재 웨이브 킬 카운트 초기화
	CurrentWaveKills = 0;

	// 보스 웨이브 판정
	bIsBossWave = (CurrentWave == FinalBossWave);

	UE_LOG(LogTemp, Log, TEXT("[GameMode] ===== Starting Wave %d ====="), CurrentWave);

	if (STGameState)
	{
		STGameState->SetCurrentWave(CurrentWave);
		STGameState->SetIsBossWave(bIsBossWave);
	}

	// 웨이브별 필요 킬 수 설정
	RequiredKillsForCurrentWave = GetRequiredKillsForWave(CurrentWave);

	// 최종 보스 웨이브
	if (bIsBossWave)
	{
		if (FinalEnemyClass)
		{
			SpawnBoss(FinalEnemyClass);
			UE_LOG(LogTemp, Warning, TEXT("[GameMode] Boss Wave! Required Kills: %d"), RequiredKillsForCurrentWave);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[GameMode] FinalEnemyClass not set!"));
		}
	}
	else
	{
		SpawnEnemies();
		UE_LOG(LogTemp, Log, TEXT("[GameMode] Wave %d - Required Kills: %d / Total Spawned: %d"),
			CurrentWave, RequiredKillsForCurrentWave, TotalEnemiesSpawned);
	}

	// 웨이브 타임리밋 시작 (5분)
	GetWorldTimerManager().SetTimer(
		WaveTimeLimitTimer,
		this,
		&ASTGameMode::OnWaveTimeLimit,
		MaxWaveTimeLimit,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[GameMode] ⏰ Wave Time Limit: %.0f seconds (%.1f minutes)"),
		MaxWaveTimeLimit, MaxWaveTimeLimit / 60.0f);
}

// 웨이브별 필요 킬 수 반환
int32 ASTGameMode::GetRequiredKillsForWave(int32 Wave)
{
	if (Wave == FinalBossWave)
	{
		return 1;  // 보스는 1마리
	}
	else if (Wave >= ZombieStartWave)
	{
		// Rampage(두 번째 적) 웨이브
		return RampageKillsRequiredForClear;  // 기본 20마리
	}
	else
	{
		// 좀비(첫 번째 적) 웨이브
		return ZombieKillsRequiredForClear;  // 기본 15마리
	}
}

int32 ASTGameMode::CalculateEnemiesForWave(int32 Wave) const
{
	int32 BaseMonsters = MonsterPerWave;
	return BaseMonsters * (1 << (Wave - 1));
}

float ASTGameMode::GetHealthMultiplierForWave(int32 Wave) const
{
	return FMath::Pow(HealthIncreaseMultiplier, Wave - 1);
}

void ASTGameMode::SpawnEnemies()
{
	if (SpawnVolumes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] No SpawnVolumes!"));
		return;
	}

	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] EnemyClass not set!"));
		return;
	}

	// 스폰할 적 수 계산
	int32 EnemiesToSpawn = CalculateEnemiesForWave(CurrentWave);
	int32 ZombiesToSpawn = 0;

	// 두 번째 적 추가 (Rampage)
	if (CurrentWave >= ZombieStartWave && SecondEnemyClass)
	{
		ZombiesToSpawn = ZombiePerWave;
		EnemiesToSpawn -= ZombiesToSpawn;
	}

	float HealthMultiplier = GetHealthMultiplierForWave(CurrentWave);

	// 일반 적 스폰
	for (int32 i = 0; i < EnemiesToSpawn; i++)
	{
		FVector SpawnLocation;
		if (GetRandomSpawnLocation(SpawnLocation))
		{
			FRotator SpawnRotation = FRotator::ZeroRotator;
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AAIMonsterBase* SpawnedEnemy = GetWorld()->SpawnActor<AAIMonsterBase>(
				EnemyClass, SpawnLocation, SpawnRotation, SpawnParams
			);

			if (SpawnedEnemy)
			{
				SpawnedEnemy->ApplyHealthMultiplier(HealthMultiplier);
				EnemiesAlive++;
				TotalEnemiesSpawned++;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnEnemies (Normal): FAILED"));
			break;
		}
	}

	// 두 번째 적 스폰
	for (int32 i = 0; i < ZombiesToSpawn; i++)
	{
		FVector SpawnLocation;
		if (GetRandomSpawnLocation(SpawnLocation))
		{
			FRotator SpawnRotation = FRotator::ZeroRotator;
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AAIMonsterBase* SpawnedZombie = GetWorld()->SpawnActor<AAIMonsterBase>(
				SecondEnemyClass, SpawnLocation, SpawnRotation, SpawnParams
			);

			if (SpawnedZombie)
			{
				SpawnedZombie->ApplyHealthMultiplier(HealthMultiplier);
				EnemiesAlive++;
				TotalEnemiesSpawned++;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnEnemies (Rampage): FAILED"));
			break;
		}
	}

	// GameState 업데이트
	if (STGameState)
	{
		STGameState->SetEnemiesRemaining(EnemiesAlive);
		STGameState->SetTotalEnemiesInWave(TotalEnemiesSpawned);
	}

	UE_LOG(LogTemp, Log, TEXT("[GameMode] Wave %d Spawned: %d enemies (Normal: %d, Rampage: %d, Health: x%.2f)"),
		CurrentWave, TotalEnemiesSpawned, EnemiesToSpawn, ZombiesToSpawn, HealthMultiplier);
}

void ASTGameMode::SpawnBoss(TSubclassOf<AAIMonsterBase> BossClassToSpawn)
{
	FVector SpawnLocation;

	if (GetRandomSpawnLocation(SpawnLocation))
	{
		FRotator SpawnRotation = FRotator::ZeroRotator;
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AAIMonsterBase* SpawnedBoss = GetWorld()->SpawnActor<AAIMonsterBase>(
			BossClassToSpawn, SpawnLocation, SpawnRotation, SpawnParams
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

			UE_LOG(LogTemp, Warning, TEXT("[GameMode] 🔥 BOSS SPAWNED! Health: x%.2f"), BossHealthMultiplier);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Boss Spawn Failed!"));
	}
}

bool ASTGameMode::GetRandomSpawnLocation(FVector& OutSpawnLocation)
{
	if (SpawnVolumes.Num() == 0)
	{
		OutSpawnLocation = FVector(0.f, 0.f, 100.f);
		return false;
	}

	const int32 RandomIndex = FMath::RandRange(0, SpawnVolumes.Num() - 1);

	if (!SpawnVolumes.IsValidIndex(RandomIndex))
	{
		OutSpawnLocation = FVector(0.f, 0.f, 100.f);
		return false;
	}

	ASTSpawnVolume* SelectedVolume = SpawnVolumes[RandomIndex];

	if (IsValid(SelectedVolume))
	{
		OutSpawnLocation = SelectedVolume->GetRandomPointInVolume();
		return true;
	}

	OutSpawnLocation = FVector(0, 0, 100);
	return false;
}

// 적 처치 시 호출
void ASTGameMode::OnEnemyKilled(AAIMonsterBase* KilledEnemy)
{
	if (bIsGameOver) return;

	// 살아있는 적 수 감소
	EnemiesAlive = FMath::Max(0, EnemiesAlive - 1);

	// 현재 웨이브 킬 수 증가
	CurrentWaveKills++;

	// 점수 추가
	int32 ScoreToAdd = bIsBossWave ? BossKillScore : ScorePerKill;
	AddScore(ScoreToAdd);

	// GameState 업데이트
	if (STGameState)
	{
		STGameState->SetEnemiesRemaining(EnemiesAlive);
	}

	// PlayerState에 킬 추가
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		ASTPlayerState* PS = PC->GetPlayerState<ASTPlayerState>();
		if (PS)
		{
			PS->AddKill();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[GameMode] 💀 Enemy Killed! Progress: %d/%d (Total Alive: %d)"),
		CurrentWaveKills, RequiredKillsForCurrentWave, EnemiesAlive);

	// 웨이브 클리어 조건 체크
	CheckWaveClearCondition();
}

// 웨이브 클리어 조건 체크
void ASTGameMode::CheckWaveClearCondition()
{
	// 필요한 킬 수를 달성했는가?
	if (CurrentWaveKills >= RequiredKillsForCurrentWave)
	{
		// 타임리밋 타이머 취소
		GetWorldTimerManager().ClearTimer(WaveTimeLimitTimer);

		if (bIsBossWave)
		{
			HandleGameWin();
		}
		else
		{
			OnWaveCleared();
		}
	}
}

// 5분 시간 초과 시 호출
void ASTGameMode::OnWaveTimeLimit()
{
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] ⏰ Wave Time Limit Reached!"));
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] Progress: %d/%d kills"), CurrentWaveKills, RequiredKillsForCurrentWave);

	// 남은 적들 모두 제거
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIMonsterBase::StaticClass(), AllEnemies);

	int32 RemovedCount = 0;
	for (AActor* Enemy : AllEnemies)
	{
		if (Enemy)
		{
			Enemy->Destroy();
			RemovedCount++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[GameMode] Removed %d remaining enemies"), RemovedCount);

	EnemiesAlive = 0;

	if (STGameState)
	{
		STGameState->SetEnemiesRemaining(0);
	}

	// 보스 웨이브에서 시간 초과 시 게임 오버
	if (bIsBossWave)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] Boss Wave Time Limit! GAME OVER!"));
		HandleGameOver();
	}
	else
	{
		// 일반 웨이브는 다음 웨이브로 자동 진행
		UE_LOG(LogTemp, Log, TEXT("[GameMode] Auto-advancing to next wave..."));
		OnWaveCleared();
	}
	KillAllRemainingEnemies();
}

void ASTGameMode::OnWaveCleared()
{
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] ✅ ===== WAVE %d CLEARED! ====="), CurrentWave);
	UE_LOG(LogTemp, Log, TEXT("[GameMode] Final Score: %d/%d kills"), CurrentWaveKills, RequiredKillsForCurrentWave);

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

	// 다음 웨이브 시작 전 딜레이
	GetWorldTimerManager().SetTimer(
		TimerHandleWaveDelay,
		this,
		&ASTGameMode::HandleWaveDelay,
		DelayBetweenWaves,
		false
	);

	KillAllRemainingEnemies();

	UE_LOG(LogTemp, Log, TEXT("[GameMode] Next wave starts in %.1f seconds..."), DelayBetweenWaves);
}

void ASTGameMode::KillAllRemainingEnemies()
{
	// 모든 적 찾기
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIMonsterBase::StaticClass(), AllEnemies);

	// 전부 제거
	int32 RemovedCount = 0;
	for (AActor* Enemy : AllEnemies)
	{
		if (Enemy)
		{
			Enemy->Destroy();
			RemovedCount++;
		}
	}

	// 로그 출력
	if (RemovedCount > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[GameMode] 🧹 Cleaned up %d remaining enemies"), RemovedCount);
	}

	// 카운트 초기화
	EnemiesAlive = 0;
	if (STGameState)
	{
		STGameState->SetEnemiesRemaining(0);
	}
}

void ASTGameMode::HandleWaveDelay()
{
	TotalEnemiesSpawned = 0;
	StartNextWave();
}

void ASTGameMode::HandleGameOver()
{
	if (bIsGameOver) return;

	bIsGameOver = true;

	// 타이머 정리
	GetWorldTimerManager().ClearTimer(WaveTimeLimitTimer);
	GetWorldTimerManager().ClearTimer(TimerHandleWaveDelay);

	UE_LOG(LogTemp, Error, TEXT("[GameMode] ☠️ ===== GAME OVER ====="));

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
	if (bIsGameOver) return;

	bIsGameOver = true;

	// 타이머 정리
	GetWorldTimerManager().ClearTimer(WaveTimeLimitTimer);
	GetWorldTimerManager().ClearTimer(TimerHandleWaveDelay);

	UE_LOG(LogTemp, Warning, TEXT("[GameMode] 🎉 ===== YOU WIN! ====="));

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

void ASTGameMode::AddScore(int32 ScoreToAdd)
{
	if (STGameState)
	{
		int32 NewScore = STGameState->GetCurrentScore() + ScoreToAdd;
		STGameState->SetScore(NewScore);
		UE_LOG(LogTemp, Log, TEXT("[GameMode] Score +%d (Total: %d)"), ScoreToAdd, NewScore);
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
			UE_LOG(LogTemp, Log, TEXT("[GameMode] 💚 Player healed: %.1f HP"), HealAmount);
		}
	}
}
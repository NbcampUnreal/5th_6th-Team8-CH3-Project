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
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] SpawnVolume Failed !"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[GameMode] FoundOut SpawnVolume %d !"), SpawnVolumes.Num());
	}

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
	else
	{

	}
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
	//���̺� +
	CurrentWave++;
	//���� ���̺�����
	bIsBossWave = (CurrentWave == FinalBossWave);

	UE_LOG(LogTemp, Log, TEXT("Start Wave !"), CurrentWave);

	if (STGameState)
	{
		STGameState->SetCurrentWave(CurrentWave);
		STGameState->SetIsBossWave(bIsBossWave);
	}
	//���� ���̺��� ���
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

	return BaseMonsters * (1 << (Wave - 1));
}

//���̺꿡 ���� ü�� ����..��...
float ASTGameMode::GetHealthMultiplierForWave(int32 Wave) const
{
	return FMath::Pow(HealthIncreaseMultiplier, Wave - 1);
}

// �� ���� ����
void ASTGameMode::SpawnEnemies()
{
	if (SpawnVolumes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] SpawnVolume�� ��� ���� ������ �� �����ϴ�!"));
		return;
	}

	if (!EnemyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] EnemyClass�� �������� �ʾҽ��ϴ�!"));
		return;
	}

	// ������ �� �� ���
	int32 EnemiesToSpawn = CalculateEnemiesForWave(CurrentWave);
	int32 ZombiesToSpawn = 0;

	// ���� ���� ���̺� �̻��̸� ���� �߰�
	if (CurrentWave >= ZombieStartWave && SecondEnemyClass)
	{
		ZombiesToSpawn = ZombiePerWave;
		EnemiesToSpawn -= ZombiesToSpawn; // �Ϲ� ���� �� ����
	}

	// ���� ���̺��� ü�� ����
	float HealthMultiplier = GetHealthMultiplierForWave(CurrentWave);

	// �Ϲ� ���� ����
	for (int32 i = 0; i < EnemiesToSpawn; i++)
	{
		FVector SpawnLocation;

		if (GetRandomSpawnLocation(SpawnLocation))
		{
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
				// ü�� ���� 
				SpawnedEnemy->ApplyHealthMultiplier(HealthMultiplier);
				EnemiesAlive++;
				TotalEnemiesSpawned++;
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("SpawnEnemies (Enemies): FAILED & STOP"));
			break;
		}
	}

	// ���� ����
	for (int32 i = 0; i < ZombiesToSpawn; i++)
	{
		FVector SpawnLocation;

		if (GetRandomSpawnLocation(SpawnLocation))
		{
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
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnEnemies (Zombie): FAILED & STOP"));
			break;
		}
	}

	// GameState ������Ʈ
	if (STGameState)
	{
		STGameState->SetEnemiesRemaining(EnemiesAlive);
		STGameState->SetTotalEnemiesInWave(TotalEnemiesSpawned);
	}

	UE_LOG(LogTemp, Log, TEXT("[GameMode] Wave %d: Finish %d Spawned (Monster: %d, Zombie: %d, Health: %.2f)"),
		CurrentWave, EnemiesAlive, EnemiesToSpawn, ZombiesToSpawn, HealthMultiplier);
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
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Boss Spawn Failed !"));
		return;
	}
}

// ���� ���� ��ġ ����
bool ASTGameMode::GetRandomSpawnLocation(FVector& OutSpawnLocation)
{
	if (SpawnVolumes.Num() == 0)
	{
		// �⺻ ��ġ ��ȯ
		OutSpawnLocation = FVector(0.f, 0.f, 100.f);
		return false;
	}

	// ���� SpawnVolume ����
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
	else {
		OutSpawnLocation = FVector(0, 0, 100);
		return false;
	}
}

//[EnemyCharacter]�� ĳ���Ͱ� �׾��� �� ȣ��
void ASTGameMode::OnEnemyKilled(AAIMonsterBase* KilledEnemy)
{
	if (bIsGameOver) return;
	//����ִ� �� �� -1
	EnemiesAlive = FMath::Max(0, EnemiesAlive - 1);
	//���� �߰�
	int32 ScoreToAdd = bIsBossWave ? BossKillScore : ScorePerKill;
	//GameState�� ���� �� �� ������Ʈ

	if (STGameState)
	{
		STGameState->SetEnemiesRemaining(EnemiesAlive);
	}
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		ASTPlayerState* PS = PC->GetPlayerState<ASTPlayerState>();
	}
	UE_LOG(LogGameMode, Log, TEXT("SUCCESSED KILL! remained enemies: %d"), EnemiesAlive, TotalEnemiesSpawned, ScoreToAdd);
	//���� ���� 0�̸� ���̺� Ŭ���� ó��
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
	//���� ���̺�
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

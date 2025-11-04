#include "STPlayerState.h"

ASTPlayerState::ASTPlayerState()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
	MaxAmmo = 30;
	CurrentAmmo = MaxAmmo;
	KillCount = 0;

}

void ASTPlayerState::BeginPlay()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	OnKillCountChanged.Broadcast(KillCount);

}

void ASTPlayerState::SetPlayerName(const FString& NewName)
{
	PlayerName = NewName;
}

void ASTPlayerState::SetHealth(float NewHealth)
{
	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);
	
	if(OldHealth != CurrentHealth)
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}
}

void ASTPlayerState::AddHealth(float HealthDelta)
{
	SetHealth(CurrentHealth + HealthDelta);
}

void ASTPlayerState::SetMaxHealth(float NewMaxHealth)
{
	if (NewMaxHealth <= 0.f) return;

	MaxHealth = NewMaxHealth;
	if (CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void ASTPlayerState::SetCurrentAmmo(int32 NewAmmo)
{
	int32 OldAmmo = CurrentAmmo;
	CurrentAmmo = FMath::Clamp(NewAmmo, 0, MaxAmmo);

	if (OldAmmo != CurrentAmmo)
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	}
}

void ASTPlayerState::AddAmmo(int32 AmmoDelta)
{
	SetCurrentAmmo(CurrentAmmo + AmmoDelta);
}

void ASTPlayerState::SetMaxAmmo(int32 NewMaxAmmo)
{
	if (NewMaxAmmo <= 0) return;

	if (CurrentAmmo > MaxAmmo)
	{
		CurrentAmmo = MaxAmmo;
	}
	OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
}

bool ASTPlayerState::UseAmmo(int32 AmmoToUse)
{
	if (CurrentAmmo < AmmoToUse)
	{
		return false;
	}
	SetCurrentAmmo(CurrentAmmo - AmmoToUse);
	return true;
}

void ASTPlayerState::AddKill()
{
	KillCount++;
	OnKillCountChanged.Broadcast(KillCount);

	UE_LOG(LogTemp, Log, TEXT("Kill Count: %d"), KillCount);
}

void ASTPlayerState::ResetKills(const FString& NewName)
{
	PlayerName = NewName;
	UE_LOG(LogTemp, Log, TEXT("Player Name: %s"), *PlayerName);
}


FString ASTPlayerState::GetPlayerName() const 
{
	const FString ParentName = Super::GetPlayerName();

	return ParentName;
}
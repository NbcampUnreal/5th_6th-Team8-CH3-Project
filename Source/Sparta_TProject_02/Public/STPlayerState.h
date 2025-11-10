#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "STPlayerState.generated.h"

//체력 변경 알람: 현재체력, 최대체력
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
//탄약 변경 알람: 현재탄약, 최대탄약
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, CurrentAmmo, int32, MaxAmmo);
//킬 수 변경: 새 킬 수
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillCountChangedSignature, int32, NewKillCount);

UCLASS()
class SPARTA_TPROJECT_02_API ASTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASTPlayerState();

	//-------UI바인딩 델리게이트---------
	UPROPERTY(BlueprintAssignable, Category="PlayerState | Events")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "PlayerState | Events")
	FOnAmmoChangedSignature OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "PlayerState | Events")
	FOnKillCountChangedSignature OnKillCountChanged;

	//---값 수정을 위한 함수 (캐릭터/ 아이템 호출용)

	//체력
	UFUNCTION(BlueprintCallable, Category = "PlayerState | Health")
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Health")
	void AddHealth(float HealthDelta);

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Health")
	void SetMaxHealth(float NewMaxHealth);

	//탄약
	UFUNCTION(BlueprintCallable, Category = "PlayerState | Ammo")
	void SetCurrentAmmo(int32 NewAmmo);

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Ammo")
	void AddAmmo(int32 AmmoDelta);

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Ammo")
	void SetMaxAmmo(int32 NewMaxAmmo);

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Ammo")
	bool UseAmmo(int32 AmmoToUse = 1);

	//킬 수
	UFUNCTION(BlueprintCallable, Category = "PlayerState | Combat")
	void AddKill();

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Combat")
	void ResetKills(const FString& NewName);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PlayerState | Config")
	int32 KillCount;

	//Getters

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	int32 GetMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	int32 GetKillCount() const { return KillCount; }

	FString GetPlayerName() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetPlayerName(const FString& NewName);

protected:
	virtual void BeginPlay() override;

	//---데이터 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerState | Identity")
	FString PlayerName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerState | Config")
	float MaxHealth;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PlayerState | Config")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerState | Config")
	int32 MaxAmmo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PlayerState | Config")
	int32 CurrentAmmo;

};

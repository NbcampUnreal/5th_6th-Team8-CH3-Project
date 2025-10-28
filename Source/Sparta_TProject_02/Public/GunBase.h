#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponType.h"
#include "Particles/ParticleSystem.h" 
#include "GunBase.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;
class APlayerCharacter;
class USoundBase;

UCLASS()
class SPARTA_TPROJECT_02_API AGunBase : public AActor
{
	GENERATED_BODY()

public:
	AGunBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float Damage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 MaxMagazineAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 CurrentAmmo;
	bool bIsReloading;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	EWeaponType WeaponType;

	// 추가: 총구 화염 파티클 (BP에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Effects")
	UParticleSystem* MuzzleFlash;

	// 추가: 발사 사운드 (BP에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Effects")
	USoundBase* FireSound;

	// 추가: 1인칭 팔 발사 몽타주 (BP에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Animation")
	UAnimMontage* FireMontage;

	// 추가: 1인칭 팔 재장전 몽타주 (BP에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Animation")
	UAnimMontage* ReloadMontage;

	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;

	// 추가: 재장전을 위해 소유 플레이어 캐릭터를 저장
	UPROPERTY()
	APlayerCharacter* OwningPlayer;


	virtual void TraceFire();

public:
	virtual void StartFire();
	virtual void StopFire();

	void Reload();
	void FinishReload();

	// 수정: 매개변수 이름 bHidden -> bShouldBeHidden으로 변경 (AActor 멤버 변수와 충돌 방지)
	void SetWeaponHidden(bool bShouldBeHidden);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void Shoot();

	USkeletalMeshComponent* GetGunMesh() const { return GunMesh; }
};
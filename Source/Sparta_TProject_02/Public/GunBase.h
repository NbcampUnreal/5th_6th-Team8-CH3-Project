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

	// �߰�: �ѱ� ȭ�� ��ƼŬ (BP���� ����)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Effects")
	UParticleSystem* MuzzleFlash;

	// �߰�: �߻� ���� (BP���� ����)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Effects")
	USoundBase* FireSound;

	// �߰�: 1��Ī �� �߻� ��Ÿ�� (BP���� ����)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Animation")
	UAnimMontage* FireMontage;

	// �߰�: 1��Ī �� ������ ��Ÿ�� (BP���� ����)
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Animation")
	UAnimMontage* ReloadMontage;

	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;

	// �߰�: �������� ���� ���� �÷��̾� ĳ���͸� ����
	UPROPERTY()
	APlayerCharacter* OwningPlayer;


	virtual void TraceFire();

public:
	virtual void StartFire();
	virtual void StopFire();

	void Reload();
	void FinishReload();

	// ����: �Ű����� �̸� bHidden -> bShouldBeHidden���� ���� (AActor ��� ������ �浹 ����)
	void SetWeaponHidden(bool bShouldBeHidden);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void Shoot();

	USkeletalMeshComponent* GetGunMesh() const { return GunMesh; }
};
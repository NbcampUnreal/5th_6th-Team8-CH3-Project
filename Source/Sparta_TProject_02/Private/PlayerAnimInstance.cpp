#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GunBase.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	Speed = 0.f;
	bIsInAir = false;
	bIsCrouching = false;
	bIsSprinting = false;
	bIsAiming = false;
	bIsReloading = false;
	bIsFiring = false;
	CurrentWeaponType = EWeaponType::WT_None;
	OwningPawn = nullptr;
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningPawn = TryGetPawnOwner();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningPawn) OwningPawn = TryGetPawnOwner();
	if (!OwningPawn) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(OwningPawn);
	if (!Player) return;

	Speed = Player->GetVelocity().Size2D();
	bIsInAir = Player->GetMovementComponent()->IsFalling();
	bIsCrouching = Player->bIsCrouched;
	bIsSprinting = Player->IsSprinting();
	bIsAiming = Player->IsAiming();
	bIsFiring = Player->IsFiring(); // << [수정 1] PlayerCharacter에서 가져온 bIsFiring 상태를 업데이트합니다.

	if (Player->GetCurrentWeapon())
	{
		bIsReloading = Player->GetCurrentWeapon()->IsReloading();
		CurrentWeaponType = Player->GetCurrentWeapon()->GetWeaponType();
	}
	else
	{
		bIsReloading = false;
		CurrentWeaponType = EWeaponType::WT_None;
	}
}
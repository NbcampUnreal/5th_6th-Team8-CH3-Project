#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "GunBase.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	CameraComp->bUsePawnControlRotation = true;

	PlayerMesh = GetMesh();
	PlayerMesh->SetupAttachment(GetCapsuleComponent());
	PlayerMesh->SetOwnerNoSee(true);
	PlayerMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.0f));

	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Mesh"));
	FP_Mesh->SetupAttachment(CameraComp);
	FP_Mesh->SetOnlyOwnerSee(true);
	FP_Mesh->SetRelativeLocation(FVector(0.f, 0.f, -150.f));
	FP_Mesh->SetCastShadow(false);

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	AmmoReserve.Add(EWeaponType::WT_Pistol, 0);
	AmmoReserve.Add(EWeaponType::WT_Rifle, 90);
	AmmoReserve.Add(EWeaponType::WT_Shotgun, 30);

	MaxCarryAmmo.Add(EWeaponType::WT_Pistol, 0);
	MaxCarryAmmo.Add(EWeaponType::WT_Rifle, 180);
	MaxCarryAmmo.Add(EWeaponType::WT_Shotgun, 60);

	CurrentWeaponIndex = -1;
	CurrentWeapon = nullptr;

	bWantsToSprint = false;
	bIsAiming = false;
	bIsFiring = false; // << [���� 1] bIsFiring �ʱ�ȭ �߰�
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (StartWeaponClasses.Num() > 0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		for (TSubclassOf<AGunBase> WeaponClass : StartWeaponClasses)
		{
			if (WeaponClass)
			{
				AGunBase* NewWeapon = GetWorld()->SpawnActor<AGunBase>(WeaponClass, SpawnParams);
				if (NewWeapon)
				{
					// --- �� �κ��� ���� ---
					FName AttachSocketName = TEXT("GripPoint"); // �⺻��

					// 1. ������ Ÿ���� �����ͼ�
					EWeaponType Type = NewWeapon->GetWeaponType(); // AGunBase�� GetWeaponType() �Լ��� �ִٰ� ����

					// 2. Ÿ�Կ� ���� ���� �̸��� ����
					if (Type == EWeaponType::WT_Rifle)
					{
						AttachSocketName = TEXT("GripPoint_Rifle");
					}
					else if (Type == EWeaponType::WT_Shotgun)
					{
						AttachSocketName = TEXT("GripPoint_Shotgun");
					}
					else if (Type == EWeaponType::WT_Pistol)
					{
						AttachSocketName = TEXT("GripPoint_Pistol");
					}

					// 3. ������ ���� �̸����� ����
					NewWeapon->AttachToComponent(
						FP_Mesh,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale,
						AttachSocketName // <-- ������ ���� �̸� ���
					);

					NewWeapon->SetWeaponHidden(true);

					Weapons.Add(NewWeapon);

					NewWeapon->OnStartFire.AddDynamic(this, &APlayerCharacter::OnWeaponStartFire);
					NewWeapon->OnStopFire.AddDynamic(this, &APlayerCharacter::OnWeaponStopFire);
					NewWeapon->OnStartReload.AddDynamic(this, &APlayerCharacter::OnWeaponStartReload);
					NewWeapon->OnFinishReload.AddDynamic(this, &APlayerCharacter::OnWeaponFinishReload);
				}
			}
		}

		EquipWeaponByType(EWeaponType::WT_Rifle);
		if (!CurrentWeapon && Weapons.Num() > 0)
		{
			EquipWeapon(0);
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction) EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		if (LookAction) EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::StartJump);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopJump);
		}

		if (SprintAction)
		{
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::StartSprint);
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
		}

		if (CrouchAction)
		{
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::StartCrouch);
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopCrouch);
		}

		if (ShootAction)
		{
			EnhancedInput->BindAction(ShootAction, ETriggerEvent::Started, this, &APlayerCharacter::StartShoot);
			EnhancedInput->BindAction(ShootAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopShoot);
		}

		if (ReloadAction)
		{
			EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, this, &APlayerCharacter::StartReload);
		}

		if (EquipShotgunAction) EnhancedInput->BindAction(EquipShotgunAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipShotgun);
		if (EquipRifleAction) EnhancedInput->BindAction(EquipRifleAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipRifle);
		if (EquipPistolAction) EnhancedInput->BindAction(EquipPistolAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipPistol);
	}
}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();
	const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (!FMath::IsNearlyZero(MoveInput.Y)) AddMovementInput(ForwardDir, MoveInput.Y);
	if (!FMath::IsNearlyZero(MoveInput.X)) AddMovementInput(RightDir, MoveInput.X);
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X * -1.0f); 
	AddControllerPitchInput(LookInput.Y);
}

void APlayerCharacter::StartJump(const FInputActionValue& value) { Jump(); }
void APlayerCharacter::StopJump(const FInputActionValue& value) { StopJumping(); }

void APlayerCharacter::StartSprint(const FInputActionValue& value)
{
	bWantsToSprint = true;
	if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}
void APlayerCharacter::StopSprint(const FInputActionValue& value)
{
	bWantsToSprint = false;
	if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void APlayerCharacter::StartCrouch(const FInputActionValue& value) { Crouch(); }
void APlayerCharacter::StopCrouch(const FInputActionValue& value) { UnCrouch(); }

void APlayerCharacter::StartShoot(const FInputActionValue& value)
{
	if (CurrentWeapon) CurrentWeapon->StartFire();
}
void APlayerCharacter::StopShoot(const FInputActionValue& value)
{
	if (CurrentWeapon) CurrentWeapon->StopFire();
}

void APlayerCharacter::StartReload(const FInputActionValue& value)
{
	if (CurrentWeapon) CurrentWeapon->Reload();
}

// Equip helpers
void APlayerCharacter::EquipShotgun(const FInputActionValue& value) { EquipWeaponByType(EWeaponType::WT_Shotgun); }
void APlayerCharacter::EquipRifle(const FInputActionValue& value) { EquipWeaponByType(EWeaponType::WT_Rifle); }
void APlayerCharacter::EquipPistol(const FInputActionValue& value) { EquipWeaponByType(EWeaponType::WT_Pistol); }

void APlayerCharacter::EquipWeaponByType(EWeaponType TypeToEquip)
{
	if (CurrentWeapon && CurrentWeapon->GetWeaponType() == TypeToEquip) return;

	for (int32 i = 0; i < Weapons.Num(); ++i)
	{
		if (Weapons[i] && Weapons[i]->GetWeaponType() == TypeToEquip)
		{
			EquipWeapon(i);
			return;
		}
	}
}

void APlayerCharacter::EquipWeapon(int32 Index)
{
	if (!Weapons.IsValidIndex(Index) || Weapons[Index] == CurrentWeapon) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire(); // << [���� 3] ���� ��ü �� ���� ���� �߻� ���� (�ִϸ��̼�/Ÿ�̸� �ʱ�ȭ�� ����)
		CurrentWeapon->SetWeaponHidden(true);
	}

	bIsFiring = false; // << [���� 3] ���� ��ü �� �߻� ���� �ʱ�ȭ

	CurrentWeapon = Weapons[Index];
	CurrentWeapon->SetWeaponHidden(false);
	CurrentWeaponIndex = Index;
}

void APlayerCharacter::AddAmmo(EWeaponType WeaponType, int32 Amount)
{
	if (Amount <= 0) return;
	if (WeaponType == EWeaponType::WT_Pistol) return;

	int32* Current = AmmoReserve.Find(WeaponType);
	int32 CurrentVal = Current ? *Current : 0;

	int32 MaxCarry = 999;
	if (int32* FoundMax = MaxCarryAmmo.Find(WeaponType)) MaxCarry = *FoundMax;

	int32 NewVal = FMath::Clamp(CurrentVal + Amount, 0, MaxCarry);
	AmmoReserve.Add(WeaponType, NewVal);

	UE_LOG(LogTemp, Log, TEXT("Added ammo: %d for %d, now %d"), Amount, (int32)WeaponType, NewVal);
}

int32 APlayerCharacter::ConsumeAmmoForReload(EWeaponType WeaponType, int32 RequestedAmount)
{
	if (WeaponType == EWeaponType::WT_Pistol) return RequestedAmount;

	int32* Current = AmmoReserve.Find(WeaponType);
	int32 CurrentVal = Current ? *Current : 0;

	int32 ToGive = FMath::Min(CurrentVal, RequestedAmount);
	int32 NewVal = CurrentVal - ToGive;
	AmmoReserve.Add(WeaponType, NewVal);

	UE_LOG(LogTemp, Log, TEXT("Consumed %d ammo for reload (%d left)"), ToGive, NewVal);
	return ToGive;
}

int32 APlayerCharacter::GetReserveAmmo(EWeaponType WeaponType) const
{
	const int32* Found = AmmoReserve.Find(WeaponType);
	return Found ? *Found : 0;
}

void APlayerCharacter::OnWeaponStartFire()
{
	bIsFiring = true; // << [���� 1] �߻� ���� ������Ʈ
	if (!CurrentWeapon) return;
	if (UAnimInstance* Anim = FP_Mesh->GetAnimInstance())
	{
		if (UAnimMontage* M = CurrentWeapon->GetFireMontage())
		{
			Anim->Montage_Play(M);
		}
	}
}

void APlayerCharacter::OnWeaponStopFire()
{
	bIsFiring = false; // << [���� 1] �߻� ���� ������Ʈ
}

void APlayerCharacter::OnWeaponStartReload()
{
	if (!CurrentWeapon) return;
	if (UAnimInstance* Anim = FP_Mesh->GetAnimInstance())
	{
		if (UAnimMontage* M = CurrentWeapon->GetReloadMontage())
		{
			Anim->Montage_Play(M);
		}
	}
}

void APlayerCharacter::OnWeaponFinishReload()
{
	// HUD ������Ʈ ��
}
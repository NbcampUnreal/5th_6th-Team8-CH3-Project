#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "GunBase.h"
#include "GrenadeActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Turret.h"
#include "Kismet/GameplayStatics.h"

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

	CurrentWeaponIndex = 0;
	CurrentWeapon = nullptr; 
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void APlayerCharacter::HealOnWaveClear(float HealAmount)
{
	if (HealAmount <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.f, MaxHealth);
	CurrentWeaponIndex = -1;
	CurrentWeapon = nullptr;

	bWantsToSprint = false;
	bIsAiming = false;
	bIsFiring = false; // 
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

		if (GrenadeAction)
		{
			EnhancedInput->BindAction(GrenadeAction, ETriggerEvent::Started, this, &APlayerCharacter::ThrowGrenade);
		}

		if (TurretAction)
		{
			EnhancedInput->BindAction(TurretAction, ETriggerEvent::Started, this, &APlayerCharacter::SpawnTurret);
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
	if (bWantsToSprint)
	{
		bWantsToSprint = false;
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		}
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
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

	if (CurrentWeapon && CurrentWeapon->IsReloading())
	{
		return;
	}

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
	if (CurrentWeapon && CurrentWeapon->IsReloading())
	{
		return;
	}
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

void APlayerCharacter::ThrowGrenade(const FInputActionValue& Value)
{
	if (!bCanThrowGrenade) return;
	if (!GrenadeClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GrenadeClass not set on PlayerCharacter"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	// 1��Ī ī�޶� ���� �߻� ����/��ġ ���
	FVector CamLoc;
	FRotator CamRot;
	if (AController* C = GetController())
	{
		// GetPlayerViewPoint ��� (ī�޶� ��ġ�� ȸ�� ���)
		C->GetPlayerViewPoint(CamLoc, CamRot);
	}
	else
	{
		CamLoc = CameraComp ? CameraComp->GetComponentLocation() : GetActorLocation();
		CamRot = CameraComp ? CameraComp->GetComponentRotation() : GetActorRotation();
	}

	// ī�޶� ���� �ణ ������ ��ġ���� ���� (�ڱ� �ڽŰ� �浹 ����)
	const float SpawnForwardOffset = 100.f;
	FVector SpawnLocation = CamLoc + CamRot.Vector() * SpawnForwardOffset;
	FRotator SpawnRotation = CamRot;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	AGrenadeActor* Grenade = World->SpawnActor<AGrenadeActor>(GrenadeClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (Grenade)
	{
		if (Grenade->ProjectileMovement)
		{
			FVector LaunchVel = CamRot.Vector() * GrenadeThrowStrength + FVector(0.f, 0.f, 200.f);
			Grenade->ProjectileMovement->Velocity = LaunchVel;
		}
		else if (Grenade->MeshComp && Grenade->MeshComp->IsSimulatingPhysics())
		{
			FVector Impulse = CamRot.Vector() * GrenadeThrowStrength;
			Grenade->MeshComp->AddImpulse(Impulse, NAME_None, true);
		}
	}

	if (ThrowGrenadeMontage)
	{
		if (UAnimInstance* AnimInst = FP_Mesh->GetAnimInstance())
		{
			AnimInst->Montage_Play(ThrowGrenadeMontage);
		}
	}

	// ======= [��ٿ� ó��] =======
	bCanThrowGrenade = false;
	GetWorldTimerManager().SetTimer(GrenadeCooldownHandle, [this]()
		{
			bCanThrowGrenade = true;
		}, GrenadeCooldown, false);

	// ======= [���� ���] =======
	if (ThrowGrenadeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(World, ThrowGrenadeSound, GetActorLocation());
	}
}


void APlayerCharacter::SpawnTurret()
{
	if (!bCanUseTurretSkill || !TurretClass) return;

	FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 200.f;
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// 아래 방향으로 1000만큼 트레이스 (지면 감지)
	FVector TraceStart = SpawnLoc;
	FVector TraceEnd = SpawnLoc - FVector(0, 0, 1000.f);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		SpawnLoc = HitResult.Location; // 땅에 맞은 지점으로 위치 보정
	}

	// 살짝 위로 띄워서 겹침 방지
	SpawnLoc.Z += 5.f;

	ATurret* NewTurret = GetWorld()->SpawnActor<ATurret>(TurretClass, SpawnLoc, GetActorRotation());
	bCanUseTurretSkill = false;

	// 30초 쿨타임
	GetWorldTimerManager().SetTimer(TurretCooldownHandle, this, &APlayerCharacter::ResetTurretCooldown, 30.0f, false);
	UE_LOG(LogTemp, Log, TEXT("Turret Spawned! Cooldown started."));
}

void APlayerCharacter::ResetTurretCooldown()
{
	bCanUseTurretSkill = true;
	UE_LOG(LogTemp, Log, TEXT("Turret skill ready again."));
}

void APlayerCharacter::OnWeaponFinishReload()
{
	// HUD ������Ʈ ��
}
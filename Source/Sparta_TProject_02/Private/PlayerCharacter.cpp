#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "InputActionValue.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "GunBase.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"

//����UI
#include "Kismet/GameplayStatics.h"
#include "Shop.h"


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
	CurrentWeapon = nullptr; // ����: CurrentGun -> CurrentWeapon

	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void APlayerCharacter::HealOnWaveClear(float HealAmount)
{
	if (HealAmount <= 0.0f)
	{
		return;
	}
	// �ִ� ü��(MaxHealth)�� ���� �ʵ��� �����ϰ� ���մϴ�.
	Health = FMath::Clamp(Health + HealAmount, 0.f, MaxHealth);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ����: ���� �ѱ� �������� �迭 ��ȸ �������� ����
	if (StartWeaponClasses.Num() > 0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		for (TSubclassOf<AGunBase> WeaponClass : StartWeaponClasses)
		{
			if (WeaponClass)
			{
				AGunBase* NewWeapon = GetWorld()->SpawnActor<AGunBase>(WeaponClass, SpawnParams);
				if (NewWeapon)
				{
					NewWeapon->AttachToComponent(
						FP_Mesh,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale,
						FName("GripPoint")
					);

					Weapons.Add(NewWeapon);
					// GunBase�� �߰��� �Լ�: ���� �� ��� ����� ����
					NewWeapon->SetWeaponHidden(true);
				}
			}
		}

		EquipWeapon(0);
	}

	//����UI
	//ĳ���Ͱ� ���忡 �����Ǿ� �ִ� AShop Ŭ������ ��ü�� ã�Ƽ� ���� (Shop ���ʹ� 1���� ���� �� �Ŷ� �ش� ����� ä����)
	if (!ShopActor)
	{
		TArray<AActor*> FoundShops;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShop::StaticClass(), FoundShops);
		if (FoundShops.Num() > 0)
		{
			ShopActor = Cast<AShop>(FoundShops[0]);
			if (APlayerController* MyPlayerController = Cast<APlayerController>(Controller))
			{
				ShopActor->SetShopPlayerController(MyPlayerController);
			}
		}
	}
}

//Open Shop UI
void APlayerCharacter::OpenShop()
{
	if (ShopActor)
	{
		ShopActor->OpenShop();
	}
}
//Close Shop UI
void APlayerCharacter::CloseShop()
{
	if (ShopActor)
	{
		ShopActor->CloseShop();
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		}

		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		}

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

		// �߰�: ���� ��ü ��ǲ ���ε�
		if (NextWeaponAction)
		{
			EnhancedInput->BindAction(NextWeaponAction, ETriggerEvent::Started, this, &APlayerCharacter::NextWeapon);
		}
		if (PrevWeaponAction)
		{
			EnhancedInput->BindAction(PrevWeaponAction, ETriggerEvent::Started, this, &APlayerCharacter::PrevWeapon);
		}
	}
}


void APlayerCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();

	const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(ForwardDir, MoveInput.Y);
	}

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(RightDir, MoveInput.X);
	}
}

void APlayerCharacter::StartJump(const FInputActionValue& value)
{
	Jump();
}

void APlayerCharacter::StopJump(const FInputActionValue& value)
{
	StopJumping();
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X * -1.0f);
	AddControllerPitchInput(LookInput.Y);
}

void APlayerCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void APlayerCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void APlayerCharacter::StartCrouch(const FInputActionValue& value)
{
	Crouch();
}

void APlayerCharacter::StopCrouch(const FInputActionValue& value)
{
	UnCrouch();
}

void APlayerCharacter::StartShoot(const FInputActionValue& value)
{
	if (CurrentWeapon) // ����: CurrentGun -> CurrentWeapon
	{
		CurrentWeapon->StartFire();
	}
}

void APlayerCharacter::StopShoot(const FInputActionValue& value)
{
	if (CurrentWeapon) // ����: CurrentGun -> CurrentWeapon
	{
		CurrentWeapon->StopFire();
	}
}

void APlayerCharacter::StartReload(const FInputActionValue& value)
{
	if (CurrentWeapon) // ����: CurrentGun -> CurrentWeapon
	{
		CurrentWeapon->Reload();
	}
}

// �߰�: ���� ���� �Լ�
void APlayerCharacter::NextWeapon(const FInputActionValue& value)
{
	if (Weapons.Num() <= 1) return; // ���Ⱑ �ϳ� ���ϸ� ��ü �� ��

	CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
	EquipWeapon(CurrentWeaponIndex);
}

// �߰�: ���� ���� �Լ�
void APlayerCharacter::PrevWeapon(const FInputActionValue& value)
{
	if (Weapons.Num() <= 1) return; // ���Ⱑ �ϳ� ���ϸ� ��ü �� ��

	CurrentWeaponIndex--;
	if (CurrentWeaponIndex < 0)
	{
		CurrentWeaponIndex = Weapons.Num() - 1;
	}
	EquipWeapon(CurrentWeaponIndex);
}

// �߰�: ���� ���� �Լ�
void APlayerCharacter::EquipWeapon(int32 Index)
{
	if (!Weapons.IsValidIndex(Index)) return;

	// ���� ���� ����
	if (CurrentWeapon)
	{
		CurrentWeapon->SetWeaponHidden(true);
	}

	// �� ���� ���� �� ǥ��
	CurrentWeapon = Weapons[Index];
	CurrentWeapon->SetWeaponHidden(false);
	CurrentWeaponIndex = Index;
}


void APlayerCharacter::AddAmmo(EWeaponType WeaponType, int32 Amount)
{
	if (Amount <= 0) return;
	if (WeaponType == EWeaponType::WT_Pistol)
		return;

	int32* Current = AmmoReserve.Find(WeaponType);
	int32 CurrentVal = Current ? *Current : 0;

	int32 MaxCarry = 999;
	if (int32* FoundMax = MaxCarryAmmo.Find(WeaponType))
	{
		MaxCarry = *FoundMax;
	}

	int32 NewVal = FMath::Clamp(CurrentVal + Amount, 0, MaxCarry);
	AmmoReserve.Add(WeaponType, NewVal);

	UE_LOG(LogTemp, Log, TEXT("Added ammo: %d for %d, now %d"), Amount, (int32)WeaponType, NewVal);
}

int32 APlayerCharacter::ConsumeAmmoForReload(EWeaponType WeaponType, int32 RequestedAmount)
{
	if (WeaponType == EWeaponType::WT_Pistol)
		return RequestedAmount;

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
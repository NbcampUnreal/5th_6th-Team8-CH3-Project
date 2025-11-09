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
#include "MyGameInstance.h"
#include "Gem.h"
#include "AttackGem.h"
#include "DefenseGem.h"
#include "SpeedGem.h"
#include "GameFramework/Controller.h" 
//상점UI
#include "Shop.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"

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

	BaseSpeed = 600.0f;
	NormalSpeed = BaseSpeed;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	AmmoReserve.Add(EWeaponType::WT_Pistol, 0);
	AmmoReserve.Add(EWeaponType::WT_Rifle, 9999);
	AmmoReserve.Add(EWeaponType::WT_Shotgun, 9999);

	MaxCarryAmmo.Add(EWeaponType::WT_Pistol, 0);
	MaxCarryAmmo.Add(EWeaponType::WT_Rifle, 9999);
	MaxCarryAmmo.Add(EWeaponType::WT_Shotgun, 9999);

	CurrentWeaponIndex = 0;
	CurrentWeapon = nullptr; 
	MaxHealth = 100.0f;
	Health = MaxHealth;
	BaseDefense = 0;
	Defense = BaseDefense;
	Attack_Increase = 0;
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
	bIsFiring = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CalculateStats();

	HandleHealthChanged(Health, MaxHealth);

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
					FName AttachSocketName = TEXT("GripPoint"); 

					EWeaponType Type = NewWeapon->GetWeaponType();

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

					NewWeapon->AttachToComponent(
						FP_Mesh,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale,
						AttachSocketName
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


	//상점UI
	//캐릭터가 월드에 스폰되어 있는 AShop 클래스의 객체를 찾아서 저장 (Shop 액터는 1개만 존재 할 거라서 해당 방법을 채택함)
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


	if (ASTPlayerState* PS = GetPlayerState<ASTPlayerState>())
	{
		PS->OnHealthChanged.AddDynamic(this, &APlayerCharacter::HandleHealthChanged);
		PS->OnAmmoChanged.AddDynamic(this, &APlayerCharacter::HandleAmmoChanged);
		PS->OnKillCountChanged.AddDynamic(this, &APlayerCharacter::HandleKillCountChanged);
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

		if (InventoryAction)
		{
			EnhancedInput->BindAction(InventoryAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleInventory);
		}

		if (OpenShopAction)
		{
			EnhancedInput->BindAction(OpenShopAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleShop);
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

void APlayerCharacter::ToggleInventory(const FInputActionValue& value)
{
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (!GameInstance) return;
	UE_LOG(LogTemp, Warning, TEXT("ToggleInventory"));
	GameInstance->ToggleInventoryWidget();
}

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
		CurrentWeapon->StopFire(); 
		CurrentWeapon->SetWeaponHidden(true);
	}

	bIsFiring = false;

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

float APlayerCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

float APlayerCharacter::GetHealth() const
{
	return Health;
}

int32 APlayerCharacter::GetBaseDefense() const
{
	return BaseDefense;
}

int32 APlayerCharacter::GetDefense() const
{
	return Defense;
}

int32 APlayerCharacter::GetAttack_Increase() const
{
	return Attack_Increase;
}

float APlayerCharacter::GetBaseSpeed() const
{
	return BaseSpeed;
}

float APlayerCharacter::GetNormalSpeed() const
{
	return NormalSpeed;
}

void APlayerCharacter::SetHealth(float NewHealth)
{
	Health = FMath::Clamp(NewHealth, Health, MaxHealth);
}

void APlayerCharacter::OnWeaponStartFire()
{
	bIsFiring = true;
}

void APlayerCharacter::OnWeaponStopFire()
{
	bIsFiring = false; 
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

	FVector CamLoc;
	FRotator CamRot;
	if (AController* C = GetController())
	{
		C->GetPlayerViewPoint(CamLoc, CamRot);
	}
	else
	{
		CamLoc = CameraComp ? CameraComp->GetComponentLocation() : GetActorLocation();
		CamRot = CameraComp ? CameraComp->GetComponentRotation() : GetActorRotation();
	}

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

	bCanThrowGrenade = false;
	GetWorldTimerManager().SetTimer(GrenadeCooldownHandle, [this]()
		{
			bCanThrowGrenade = true;
		}, GrenadeCooldown, false);

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

	FVector TraceStart = SpawnLoc;
	FVector TraceEnd = SpawnLoc - FVector(0, 0, 1000.f);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		SpawnLoc = HitResult.Location; 
	}

	SpawnLoc.Z += 5.f;

	ATurret* NewTurret = GetWorld()->SpawnActor<ATurret>(TurretClass, SpawnLoc, GetActorRotation());
	bCanUseTurretSkill = false;

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
}

bool APlayerCharacter::CalculateStats()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("CalculateStats"));
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (!GameInstance) return false;

	Attack_Increase = 0;
	int32 Defense_Increase = 0;
	int32 Speed_Increase = 0;

	UInventory* GemSlots = GameInstance->GemSlots;
	for (int32 i = 0; i < GemSlots->GetCurrentSize(); ++i)
	{
		UItem* Item = GemSlots->GetItem(i);
		if (!Item) continue;

		UEquipmentItem* EquipmentItem = Cast<UEquipmentItem>(Item);
		if (!EquipmentItem) continue;

		UGem* Gem = Cast<UGem>(EquipmentItem);
		if (!Gem) continue;

		if (UAttackGem* ATKGem = Cast<UAttackGem>(Gem))
		{
			Attack_Increase += ATKGem->GetAttackValue();
		}
		else if (UDefenseGem* DEFGem = Cast<UDefenseGem>(Gem))
		{
			Defense_Increase += DEFGem->GetDefenseValue();
		}
		else if (USpeedGem* SPDGem = Cast<USpeedGem>(Gem))
		{
			Speed_Increase += SPDGem->GetSpeedValue();
		}
	}
	// Attack_Increase = Attack_Increase;
	Defense = BaseDefense + Defense_Increase;
	Defense_Increase = BaseDefense + Defense_Increase;
	NormalSpeed = BaseSpeed + (float)Speed_Increase;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;



	return true;
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	float EffectiveDefense = (float)Defense;
	float DamageAfterDefense = DamageAmount * (100.0f / (100.0f + EffectiveDefense));
	float DamageApplied = FMath::Clamp(DamageAfterDefense, 0.0f, Health);

	Health = FMath::Clamp(Health - DamageApplied, 0.0f, MaxHealth);

	HandleHealthChanged(Health, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Player took %f damage (after def %f). Health now %f/%f"), DamageAmount, DamageApplied, Health, MaxHealth);

	if (Health <= 0.0f)
	{
		Die();
	}

	return DamageApplied;
}

void APlayerCharacter::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		DisableInput(PC);
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}

	if (PlayerMesh)
	{
	}

	UE_LOG(LogTemp, Warning, TEXT("Player Died."));

	OnDeath();
}

void APlayerCharacter::OpenShop()
{
	if (ShopActor)
	{
		ShopActor->OpenShop();
	}
}

void APlayerCharacter::CloseShop()
{
	if (ShopActor)
	{
		ShopActor->CloseShop();
	}
}

void APlayerCharacter::ToggleShop()
{
	if (!ShopActor) return;

	if (ShopActor->IsShopVisible())
	{
		CloseShop();
	}
	else
	{
		OpenShop();
	}
}
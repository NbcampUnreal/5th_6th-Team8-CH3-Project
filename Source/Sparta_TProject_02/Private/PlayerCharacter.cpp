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


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);


	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	CameraComp->bUsePawnControlRotation = true; 

	GetMesh()->SetupAttachment(GetCapsuleComponent());

	GetMesh()->SetOnlyOwnerSee(true); 
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.0f)); 

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
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultGunClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		CurrentGun = GetWorld()->SpawnActor<AGunBase>(DefaultGunClass, SpawnParams);

		if (CurrentGun)
		{
			// FP_Mesh (1ÀÎÄª ÆÈ)ÀÇ "GripPoint" ¼ÒÄÏ¿¡ ÃÑÀ» ºÎÂø
			// (FP_Mesh ½ºÄÌ·¹Åæ¿¡ "GripPoint" ¼ÒÄÏÀÌ ÀÖ¾î¾ß ÇÔ)
			CurrentGun->AttachToComponent(
				FP_Mesh,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				FName("GripPoint")
			);
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(
				MoveAction,
				ETriggerEvent::Triggered,
				this,
				&APlayerCharacter::Move
			);
		}

		if (LookAction)
		{
			EnhancedInput->BindAction(
				LookAction,
				ETriggerEvent::Triggered,
				this,
				&APlayerCharacter::Look
			);
		}

		if (JumpAction)
		{
			EnhancedInput->BindAction(
				JumpAction,
				ETriggerEvent::Started,
				this,
				&APlayerCharacter::StartJump
			);
			EnhancedInput->BindAction(
				JumpAction,
				ETriggerEvent::Completed,
				this,
				&APlayerCharacter::StopJump
			);
		}

		if (SprintAction)
		{
			EnhancedInput->BindAction(
				SprintAction,
				ETriggerEvent::Started,
				this,
				&APlayerCharacter::StartSprint
			);

			EnhancedInput->BindAction(
				SprintAction,
				ETriggerEvent::Completed,
				this,
				&APlayerCharacter::StopSprint
			);
		}
		if (CrouchAction)
		{
			EnhancedInput->BindAction(
				CrouchAction,
				ETriggerEvent::Started,
				this,
				&APlayerCharacter::StartCrouch
			);
			EnhancedInput->BindAction(
				CrouchAction,
				ETriggerEvent::Completed,
				this,
				&APlayerCharacter::StopCrouch
			);
		}

		if (ShootAction)
		{
			EnhancedInput->BindAction(
				ShootAction,
				ETriggerEvent::Started,
				this,
				&APlayerCharacter::StartShoot
			);
			EnhancedInput->BindAction(
				ShootAction,
				ETriggerEvent::Completed,
				this,
				&APlayerCharacter::StopShoot
			);
		}

		if (ReloadAction)
		{
			EnhancedInput->BindAction(
				ReloadAction,
				ETriggerEvent::Started,
				this,
				&APlayerCharacter::StartReload
			);
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
	if (CurrentGun)
	{
		CurrentGun->StartFire();
	}
}

void APlayerCharacter::StopShoot(const FInputActionValue& value)
{
	if (CurrentGun)
	{
		CurrentGun->StopFire();
	}
}

void APlayerCharacter::StartReload(const FInputActionValue& value)
{
	if (CurrentGun)
	{
		CurrentGun->Reload();
	}
}
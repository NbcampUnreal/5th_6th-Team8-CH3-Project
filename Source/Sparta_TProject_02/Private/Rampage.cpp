#include "Rampage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/CapsuleComponent.h"

ARampage::ARampage()
{
	// Rampage의 기본 스탯 설정
	MaxHealth = 1500.0f;
	AttackDamage = 75.0f;
	AttackRange = 300.0f;
}

void ARampage::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacterMovement())
	{
		OriginalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}

	USkeletalMeshComponent* RampageMesh = GetMesh();
	if (RampageMesh)
	{
		int32 MaterialCount = RampageMesh->GetNumMaterials();
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Rampage has %d materials."), MaterialCount); // 로그 추가 1

		for (int32 i = 0; i < MaterialCount; ++i)
		{
			UMaterialInterface* CurrentMaterial = RampageMesh->GetMaterial(i);
			if (CurrentMaterial)
			{
				UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(CurrentMaterial, this);
				RampageMesh->SetMaterial(i, DynMat);
				DynamicMaterialInstances.Add(DynMat);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Created %d dynamic material instances."), DynamicMaterialInstances.Num()); // 로그 추가 2
	}
}

void ARampage::PerformSmashAttack()
{
	if (bIsDead) return;

	UE_LOG(LogTemp, Warning, TEXT("Rampage [%s]: Performing SMASH Attack!"), *GetName());

	UGameplayStatics::ApplyRadialDamage(
		GetWorld(), 120.0f, GetActorLocation(), 550.0f,
		nullptr, TArray<AActor*>(), this, GetController(), true, ECC_Pawn
	);
}

void ARampage::Energize()
{
	if (bIsDead || GetWorldTimerManager().IsTimerActive(EnergizeTimerHandle)) return;

	UE_LOG(LogTemp, Warning, TEXT("Rampage [%s]: ENERGIZED!"), *GetName());

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsEnergized"), true);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed * 1.7f;
	}

	const FLinearColor EnergizeColor = FLinearColor(2.5f, 0.1f, 0.1f, 1.0f);
	UE_LOG(LogTemp, Warning, TEXT("Energize: Attempting to change color on %d materials."), DynamicMaterialInstances.Num());
	for (UMaterialInstanceDynamic* DynMat : DynamicMaterialInstances)
	{
		if (DynMat)
		{
			// 헤더 파일에 정의된 "Enrage State Tint" 이름의 파라미터 값을 변경합니다.
			UE_LOG(LogTemp, Warning, TEXT("Energize: Setting Vector Parameter on %s"), *DynMat->GetName());
			DynMat->SetVectorParameterValue(ColorOverlayParamName, EnergizeColor);
		}
	}

	GetWorldTimerManager().SetTimer(EnergizeTimerHandle, this, &ARampage::EndEnergize, 7.0f, false);
}

void ARampage::EndEnergize()
{
	UE_LOG(LogTemp, Warning, TEXT("Rampage [%s]: Energize buff ended."), *GetName());

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsEnergized"), false);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed;
	}

	const FLinearColor DefaultColor = FLinearColor::White;
	for (UMaterialInstanceDynamic* DynMat : DynamicMaterialInstances)
	{
		if (DynMat)
		{
			DynMat->SetVectorParameterValue(ColorOverlayParamName, DefaultColor);
		}
	}
}

void ARampage::Die()
{
	if (bIsDead) return;

	bIsDead = true;
	UE_LOG(LogTemp, Warning, TEXT("Rampage [%s] has died. Playing death animation."), *GetName());

	// 1. 행동 트리를 정지시킵니다.
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic("Died");
	}

	// 2. 캡슐 컴포넌트의 충돌을 꺼서 죽은 뒤에 길을 막지 않도록 합니다.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 3. 래그돌 대신 죽음 애니메이션 몽타주를 재생합니다.
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	SetLifeSpan(7.0f);
}
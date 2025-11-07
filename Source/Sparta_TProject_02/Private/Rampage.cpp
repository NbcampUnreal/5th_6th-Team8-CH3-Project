#include "Rampage.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


ARampage::ARampage()
{
	// --- Rampage의 스탯 설정 ---
	MaxHealth = 2000.0f;	// Rampge 체력
	AttackDamage = 50.0f;	// Rampage 데미지
	AttackRange = 300.0f;	// 공격범위(근접)
	Defense = 5.0f;			// 방어력
}

// --- 게임 시작시 --- 
void ARampage::BeginPlay()
{
	Super::BeginPlay();

	// 원본 스탯 저장
	if (GetCharacterMovement())
	{
		OriginalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}
	OriginalDefense = Defense;

	// 동적 머티리얼 인스턴스 생성 
	USkeletalMeshComponent* RampageMesh = GetMesh();
	if (RampageMesh)
	{
		int32 MaterialCount = RampageMesh->GetNumMaterials();
		// UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Rampage has %d materials."), MaterialCount);

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
		// UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Created %d dynamic material instances."), DynamicMaterialInstances.Num());
	}
}

// --- 사망 처리 ---
void ARampage::Die()
{
	if (bIsDead) return;
	bIsDead = true;
	// UE_LOG(LogTemp, Warning, TEXT("Rampage [%s] has died. Playing death animation."), *GetName());

	// AI의 행동 로직을 즉시 중지
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic("Died");
	}

	// 캡슐 컴포넌트의 충돌(Collision)을 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 죽음 애니메이션 몽타주를 재생합니다.
	// 래그돌 비활성화
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// 일정 시간 후 월드에서 액터를 제거
	SetLifeSpan(7.0f);
}

//==================================================
//--- Skills ---
//==================================================

// --- 광역 공격(Smash) ---
void ARampage::PerformSmashAttack()
{
	if (bIsDead) return;
	// UE_LOG(LogTemp, Warning, TEXT("Rampage [%s]: Performing SMASH Attack!"), *GetName());

	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		120.0f,
		GetActorLocation(),
		550.0f,
		nullptr,
		TArray<AActor*>(),
		this,
		GetController(),
		true,
		ECC_Pawn
	);
}

// --- 분노 버프 활성화 ---
void ARampage::Energize()
{
	if (bIsDead || GetWorldTimerManager().IsTimerActive(EnergizeTimerHandle)) return;
	// UE_LOG(LogTemp, Warning, TEXT("Rampage [%s]: ENERGIZED!"), *GetName());

	// 행동 트리가 현재 버프 상태임을 알 수 있도록 블랙보드 값 설정
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsEnergized"), true);
	}

	// 스탯 강화(이동속도, 방어력)
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed * 1.7f;
	}
	Defense = OriginalDefense * 2.0f;
	// UE_LOG(LogTemp, Warning, TEXT("Rampage [%s] Defense increased to: %.1f"), *GetName(), Defense);

	// 머티리얼 색상을 변경하여 버프 상태 표시
	const FLinearColor EnergizeColor = FLinearColor(2.5f, 0.1f, 0.1f, 1.0f);
	// UE_LOG(LogTemp, Warning, TEXT("Energize: Attempting to change color on %d materials."), DynamicMaterialInstances.Num());
	for (UMaterialInstanceDynamic* DynMat : DynamicMaterialInstances)
	{
		if (DynMat)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Energize: Setting Vector Parameter on %s"), *DynMat->GetName());
			DynMat->SetVectorParameterValue(ColorOverlayParamName, EnergizeColor);
		}
	}

	// 지정된 시간(7초) 후에 버프 해제 함수(EndEnergize)를 호출하도록 타이머 설정
	GetWorldTimerManager().SetTimer(EnergizeTimerHandle, this, &ARampage::EndEnergize, 7.0f, false);
}

// --- 분노 버프 비활성화 ---
void ARampage::EndEnergize()
{
	// UE_LOG(LogTemp, Warning, TEXT("Rampage [%s]: Energize buff ended."), *GetName());

	// 블랙보드 값을 원래대로 되돌림
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsEnergized"), false);
	}

	// 스탯을 원래 값으로 복구
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed;
	}
	Defense = OriginalDefense;
	// UE_LOG(LogTemp, Warning, TEXT("Rampage [%s] Defense reverted to: %.1f"), *GetName(), Defense);

	// 머티리얼 색상을 원래대로 되돌림
	const FLinearColor DefaultColor = FLinearColor::White;
	for (UMaterialInstanceDynamic* DynMat : DynamicMaterialInstances)
	{
		if (DynMat)
		{
			DynMat->SetVectorParameterValue(ColorOverlayParamName, DefaultColor);
		}
	}
}


#include "AIMonsterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "PlayerCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "MonsterHealthBar.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"
#include "PlayerCharacterController.h"


AAIMonsterBase::AAIMonsterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // --- 기본 스탯 초기화 ---
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 10.0f;
    AttackRange = 150.0f;
    bIsDead = false;

    // --- 체력바 위젯 컴포넌트 설정 ---
    HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidgetComponent->SetupAttachment(GetMesh()); // 메시(머리)에 붙이거나 RootComponent에 붙임
    HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // 항상 플레이어의 화면을 바라보도록 설정
    HealthBarWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f)); // 몬스터 머리 위로 위치 조정
    HealthBarWidgetComponent->SetDrawSize(FVector2D(150.0f, 20.0f)); // 체력바 크기 조정
    HealthBarWidgetComponent->SetVisibility(false); // 평소에는 숨김

    // --- 충돌체 범위 설정 ---
    AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
    AggroSphere->SetupAttachment(RootComponent);
    AggroSphere->SetCollisionProfileName(TEXT("Trigger")); // 물리적 충돌 없이 겹침(Overlap) 이벤트만 발생
    AggroSphere->SetSphereRadius(500.0f); // 인지 범위 (5미터). 이 값은 블루프린트에서 조절 가능
    AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AAIMonsterBase::OnAggroSphereOverlap); // Overlap 이벤트가 발생했을 때 호출될 함수를 연결(바인딩)합니다.

    // --- 컨트롤러 점유 설정 ---
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // 레벨에 미리 배치되거나(Placed in World), 게임 중에 스폰될 때(Spawned) 모두
}

// --- 게임 시작 시 ---
void AAIMonsterBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth; // 체력 초기화

    // 위젯 컴포넌트로부터 실제 위젯 객체를 가져와서 C++ 클래스로 캐스팅
    if (HealthBarWidgetComponent)
    {
        HealthBarWidget = Cast<UMonsterHealthBar>(HealthBarWidgetComponent->GetUserWidgetObject());
        if (HealthBarWidget)
        {
            // 처음에는 체력이 100%이므로 1.0으로 업데이트
            HealthBarWidget->UpdateHealthBar(1.0f);
        }
    }
}

void AAIMonsterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float AAIMonsterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 이미 죽었다면 데미지 처리 안 함
    if (bIsDead) return 0.0f;

    // 방어력을 적용하여 최종 데미지 계산. 최소 1의 데미지
    float ActualDamage = FMath::Max(1.0f, DamageAmount - Defense);

    if (ActualDamage > 0.0f)
    {
        // 체력을 ActualDamage 만큼 감소
        // UE_LOG(LogTemp, Warning, TEXT("%s's Health: %f"), *GetName(), CurrentHealth);
        CurrentHealth -= ActualDamage;

        // 피해를 입는 경우 AI Perception System 직접 보고
        if (DamageCauser)
        {
            UAISense_Damage::ReportDamageEvent(
                GetWorld(),         // 월드 컨텍스트
                this,               // 피해를 입은 액터 (나 자신)
                DamageCauser,       // 피해를 가한 액터 (총알 등)
                ActualDamage,       // 데미지 양
                DamageCauser->GetActorLocation(), // 피해 발생 위치 (가해자 위치)
                GetActorLocation()  // 피격 위치 (나 자신의 위치)
            );
        }

        // 체력이 0 이하가 되면 죽음, 그렇지 않으면 피격 반응
        if (CurrentHealth <= 0.0f)
        {
            if (EventInstigator)
            {
                if (APlayerCharacterController* PC = Cast<APlayerCharacterController>(EventInstigator))
                {
                    if (ASTPlayerState* PS = Cast<ASTPlayerState>(PC->PlayerState))
                    {
                        PS->AddKill();
                    }
                }
            }
            Die();
        }
        else
        {
            if (HitReactMontage)
            {
                PlayAnimMontage(HitReactMontage);
            }
        }

        // 체력바 UI 업데이트
        if (HealthBarWidget)
        {
            HealthBarWidgetComponent->SetVisibility(true); // 체력바를 보이게 함
            const float HealthRatio = CurrentHealth / MaxHealth; 
            HealthBarWidget->UpdateHealthBar(HealthRatio); // 현재 체력 비율만큼 위젯을 업데이트.
            GetWorldTimerManager().SetTimer(HealthBarTimerHandle, this, &AAIMonsterBase::HideHealthBar, 3.0f, false); // 3초 뒤에 체력바를 다시 숨기도록 타이머를 설정
        }
    }

    // 반환값
    return ActualDamage;
}

void AAIMonsterBase::Attack()
{
    // 이미 죽었다면 공격 처리 안함
    if (bIsDead) return;

    // 몬스터의 정면 방향으로 Attack Range만큼 지점까지 구체의 충돌을 감지함 
    FVector StartPoint = GetActorLocation();
    FVector EndPoint = StartPoint + (GetActorForwardVector() * AttackRange);
    FCollisionShape Sphere = FCollisionShape::MakeSphere(50.0f);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    TArray<FHitResult> HitResults;

    bool bIsHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        StartPoint,
        EndPoint,
        FQuat::Identity,
        ECC_Pawn,
        Sphere,
        Params
    );

    if (bIsHit)
    {
        for (const auto& Hit : HitResults)
        {
            APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Hit.GetActor());
            if (PlayerCharacter)
            {
                UE_LOG(LogTemp, Warning, TEXT("%s's attack hit %s! Applying %f damage."), *GetName(), *PlayerCharacter->GetName(), AttackDamage);

                UGameplayStatics::ApplyDamage(
                    PlayerCharacter,
                    AttackDamage,
                    GetController(),
                    this,
                    nullptr
                );
                break;
            }
        }
    }
}

void AAIMonsterBase::Die()
{
    // 이미 죽었으면 죽음 처리 안함
    // UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());
    if (bIsDead) return;
    bIsDead = true;

    // AI 컨트롤러의 BrainComponent 로직을 정지(Died)
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController && AIController->GetBrainComponent())
    {
        AIController->GetBrainComponent()->StopLogic("Died");
    }

    // 캡슐 컴포넌트의 충돌(Collision)을 비활성화
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

    // 스켈레탈 메시에 래그돌 물리 시뮬레이션 적용
    USkeletalMeshComponent* MyMesh = GetMesh();
    if (MyMesh)
    {
        MyMesh->SetSimulatePhysics(true);
        MyMesh->SetCollisionProfileName(TEXT("Ragdoll"));
    }

    // 죽음 애니메이션 몽타주 재생
    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }

    // 일정 시간 후 월드에서 액터를 제거
    SetLifeSpan(7.0f);
}

void AAIMonsterBase::ApplyHealthMultiplier(float Multiplier)
{
    MaxHealth *= Multiplier;
    CurrentHealth = MaxHealth;
}

void AAIMonsterBase::HideHealthBar()
{
    if (HealthBarWidgetComponent)
    {
        HealthBarWidgetComponent->SetVisibility(false);
    }
}

void AAIMonsterBase::OnAggroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 자기 자신이나 유효하지 않은 액터는 무시
    if (OtherActor == nullptr || OtherActor == this)
    {
        return;
    }

    // 겹친 대상이 플레이어 컨트롤러가 조종하는 Pawn인지 확인
    APawn* OverlappedPawn = Cast<APawn>(OtherActor);
    if (OverlappedPawn && OverlappedPawn->GetController() && OverlappedPawn->GetController()->IsA<APlayerController>())
    {
        // UE_LOG(LogTemp, Warning, TEXT("[%s] detected Player [%s] in Aggro Sphere!"), *GetName(), *OtherActor->GetName());

        // AI 컨트롤러의 블랙보드에 TargetActor를 설정합니다.
        AAIController* AIController = Cast<AAIController>(GetController());
        if (AIController)
        {
            UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
            if (BlackboardComp)
            {
                // 이미 타겟이 설정되어 있다면 갱신하지 않음
                if (BlackboardComp->GetValueAsObject(TEXT("TargetActor")) == nullptr)
                {
                    BlackboardComp->SetValueAsObject(TEXT("TargetActor"), OtherActor);
                }
            }
        }
    }
}



#include "AIMonsterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "PlayerCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "MonsterHealthBar.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"


AAIMonsterBase::AAIMonsterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // 기본값 설정
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 10.0f;
    AttackRange = 150.0f;
    bIsDead = false;

    HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidgetComponent->SetupAttachment(GetMesh()); // 메시(머리)에 붙이거나 RootComponent에 붙임
    HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // 항상 화면을 바라보도록 설정
    HealthBarWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f)); // 몬스터 머리 위로 위치 조정
    HealthBarWidgetComponent->SetDrawSize(FVector2D(150.0f, 20.0f)); // 체력바 크기 조정
    HealthBarWidgetComponent->SetVisibility(false); // 평소에는 보이지 않도록 설정

    AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
    AggroSphere->SetupAttachment(RootComponent);
    AggroSphere->SetCollisionProfileName(TEXT("Trigger")); // 물리적 충돌 없이 겹침(Overlap) 이벤트만 발생
    AggroSphere->SetSphereRadius(500.0f); // 인지 범위 (5미터). 이 값은 블루프린트에서 조절 가능

    // Overlap 이벤트가 발생했을 때 호출될 함수를 연결(바인딩)합니다.
    AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AAIMonsterBase::OnAggroSphereOverlap);
}

void AAIMonsterBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth; // 게임 시작 시 체력 초기화

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

    // 부모 클래스의 TakeDamage 먼저 호출
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.0f)
    {
        CurrentHealth -= ActualDamage;
        UE_LOG(LogTemp, Warning, TEXT("%s's Health: %f"), *GetName(), CurrentHealth);

        // 데미지를 입었다는 사실을 Perception System에 직접 보고(report)합니다.
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

        if (CurrentHealth <= 0.0f)
        {
            Die();
        }
        else
        {
            if (HitReactMontage)
            {
                PlayAnimMontage(HitReactMontage);
            }
        }
        if (HealthBarWidget)
        {
            // 1. 체력바를 보이게 합니다.
            HealthBarWidgetComponent->SetVisibility(true);

            // 2. 현재 체력 비율을 계산하여 위젯을 업데이트합니다.
            const float HealthRatio = CurrentHealth / MaxHealth;
            HealthBarWidget->UpdateHealthBar(HealthRatio);

            // 3. 3초 뒤에 체력바를 다시 숨기도록 타이머를 설정(또는 재설정)합니다.
            GetWorldTimerManager().SetTimer(HealthBarTimerHandle, this, &AAIMonsterBase::HideHealthBar, 3.0f, false);
        }
    }

    return ActualDamage;
}

void AAIMonsterBase::Attack()
{
    if (bIsDead) return;

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
    if (bIsDead) return;

    bIsDead = true;
    UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());

    // 1. AI 컨트롤러 가져오기
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController && AIController->GetBrainComponent())
    {
        // 2. 행동 트리(BrainComponent) 로직을 즉시 중지
        AIController->GetBrainComponent()->StopLogic("Died");
    }

    // 3. 충돌 비활성화
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

    // 5. 래그돌 물리 시뮬레이션
    USkeletalMeshComponent* MyMesh = GetMesh();
    if (MyMesh)
    {
        MyMesh->SetSimulatePhysics(true);
        MyMesh->SetCollisionProfileName(TEXT("Ragdoll"));
    }

    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }
    SetLifeSpan(7.0f);
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

    // 겹친 대상이 플레이어인지 확인합니다. (컨트롤러 확인 방식 사용)
    APawn* OverlappedPawn = Cast<APawn>(OtherActor);
    if (OverlappedPawn && OverlappedPawn->GetController() && OverlappedPawn->GetController()->IsA<APlayerController>())
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] detected Player [%s] in Aggro Sphere!"), *GetName(), *OtherActor->GetName());

        // AI 컨트롤러의 블랙보드에 TargetActor를 설정합니다.
        AAIController* AIController = Cast<AAIController>(GetController());
        if (AIController)
        {
            UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
            if (BlackboardComp)
            {
                // 이미 타겟이 설정되어 있다면 굳이 다시 설정할 필요는 없습니다.
                if (BlackboardComp->GetValueAsObject(TEXT("TargetActor")) == nullptr)
                {
                    BlackboardComp->SetValueAsObject(TEXT("TargetActor"), OtherActor);
                }
            }
        }
    }
}

void AAIMonsterBase::ApplyHealthMultiplier(float Multiplier)
{
    MaxHealth *= Multiplier;
    CurrentHealth = MaxHealth;
}

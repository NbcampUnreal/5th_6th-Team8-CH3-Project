// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Monster.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

// --- AI 설정값 ---
AAIC_Monster::AAIC_Monster()
{
    // Perception Component 생성
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    // 시각(Sight) 감각 설정
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        SightConfig->SightRadius = 3000.0f;                 // 시야 반경
        SightConfig->LoseSightRadius = 3100.0f;             // 시야를 잃는 반경 (더 크게 설정해야 안정적)
        SightConfig->PeripheralVisionAngleDegrees = 75.0f;  // 시야각 (~90)
        SightConfig->SetMaxAge(5.0f);                       // 감지 정보를 5초간 기억함

        // 모든 종류의 대상을 시야에 등록하도록 설정, 이후 컨트롤러 타입으로 필터링 
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

        AIPerceptionComponent->ConfigureSense(*SightConfig);

    }

    // 청각(Hearing) 설정
    UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    if (HearingConfig)
    {
        HearingConfig->HearingRange = 1500.0f;                          // 청력 반경
        HearingConfig->DetectionByAffiliation.bDetectEnemies = true;    // 적이 내는 소리만 듣도록 설정
        HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
        HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
        AIPerceptionComponent->ConfigureSense(*HearingConfig);
    }

    // 피해(Damage) 감각 설정
    UAISenseConfig_Damage* DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
    if (DamageConfig)
    {
        AIPerceptionComponent->ConfigureSense(*DamageConfig);
    }

    // 주된 감각(Dominant Sense)을 시각으로 설정
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    // 감각 정보 업데이트 시 함수 호출
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIC_Monster::OnTargetPerceptionUpdated);
}


void AAIC_Monster::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    // 행동 트리를 실행하는 로직 추가 가능
    // 현재는 BP로 구성
}

// 감각 정보 업데이트 시 실행될 함수
void AAIC_Monster::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // 폰, 블랙보드 유효성 검증
    if (GetPawn() == nullptr || GetBlackboardComponent() == nullptr)
    {
        return;
    }

    /*const FAISenseID SenseID = Stimulus.Type;
    if (SenseID == UAISense::GetSenseID<UAISense_Sight>())
    {
        UE_LOG(LogTemp, Warning, TEXT("Perception: SIGHT stimulus received from %s"), *Actor->GetName());
    }
    else if (SenseID == UAISense::GetSenseID<UAISense_Damage>())
    {
        UE_LOG(LogTemp, Warning, TEXT("Perception: DAMAGE stimulus received from %s"), *Actor->GetName());
    }
    else if (SenseID == UAISense::GetSenseID<UAISense_Hearing>())
    {
        UE_LOG(LogT emp, Warning, TEXT("Perception: HEARING stimulus received from %s"), *Actor->GetName());
    }*/

    // 감지된 액터를 '플레이어 캐릭터 클래스'로 캐스팅 시도
    UBlackboardComponent* MyBlackboard = GetBlackboardComponent();

    if (Stimulus.WasSuccessfullySensed())
    {
        // 감지된 Actor가 Pawn 타입인지 확인 
        APawn* DetectedPawn = Cast<APawn>(Actor);
        if (DetectedPawn)
        {
            // 감지된 Pawn의 컨트롤러가 PlayerController인지 확인
            if (DetectedPawn->GetController() && DetectedPawn->GetController()->IsA<APlayerController>())
            {
                // 플레이어가 조종하는 Pawn이 확실하므로, 블랙보드의 'TargetActor', 'LastKnownLocation' 키를 업데이트
                MyBlackboard->SetValueAsObject(TEXT("TargetActor"), Actor);
                MyBlackboard->SetValueAsVector(TEXT("LastKnownLocation"), Actor->GetActorLocation());
            }
        }
    }
    else
    {
        // 감지 실패 - 시야에서 놓치는 경우 등
        // 현재 추적하던 타겟인지 확인
        AActor* CurrentTarget = Cast<AActor>(MyBlackboard->GetValueAsObject(TEXT("TargetActor")));
        if (CurrentTarget == Actor)
        {
            // 블랙보드의 'TargetActor' 키를 비움
            MyBlackboard->ClearValue(TEXT("TargetActor"));
        }
    }
}

void AAIC_Monster::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsRotatingToTarget)
    {   
        ACharacter* ControlledPawn = Cast<ACharacter>(GetPawn());
        AActor* TargetActor = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));

        if (ControlledPawn && TargetActor)
        {
            // 목표물을 향하는 방향 벡터 계산, 수직 축은 무시, 목표 회전값(Rotation) 계산
            FVector LookVector = TargetActor->GetActorLocation() - ControlledPawn->GetActorLocation();
            LookVector.Z = 0.f; 
            FRotator TargetRotation = LookVector.Rotation();

            // 부드러운 회전을 위해 보간(Interpolation) 사용
            // RInterpTo(현재 회전값, 목표 회전값, 경과 시간, 회전 속도)
            FRotator NewRotation = FMath::RInterpTo(ControlledPawn->GetActorRotation(), TargetRotation, DeltaSeconds, 5.0f);

            // Pawn의 회전값을 직접 설정하는 대신, 컨트롤러의 회전값을 설정
            // Pawn의 bUseControllerRotationYaw 설정에 따라 Pawn이 부드럽게 따라 회전함
            SetControlRotation(NewRotation);
        }
    }
}

// ==================================================
// --- Rotation Control (회전 제어) ---
// ==================================================

void AAIC_Monster::StartRotatingToTarget()
{
    bIsRotatingToTarget = true;

    if (ACharacter* ControlledPawn = Cast<ACharacter>(GetPawn()))
    {
        // 캐릭터의 회전 제어권을 '이동방향'에서 '컨트롤러'로 전환
        ControlledPawn->GetCharacterMovement()->bOrientRotationToMovement = false;
        ControlledPawn->GetCharacterMovement()->bUseControllerDesiredRotation = true;
        ControlledPawn->bUseControllerRotationYaw = true;
    }
}

void AAIC_Monster::StopRotatingToTarget()
{
    bIsRotatingToTarget = false;

    // 캐릭터의 회전 제어권을 '컨트롤러'에서 '이동방향'으로 전환
    if (ACharacter* ControlledPawn = Cast<ACharacter>(GetPawn()))
    {
        ControlledPawn->GetCharacterMovement()->bOrientRotationToMovement = true;
        ControlledPawn->GetCharacterMovement()->bUseControllerDesiredRotation = false;
        ControlledPawn->bUseControllerRotationYaw = false;
    }
}
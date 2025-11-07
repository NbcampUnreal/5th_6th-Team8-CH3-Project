// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Monster.h"
#include "Perception/AIPerceptionComponent.h" // Perception 사용
#include "Perception/AISenseConfig_Sight.h" // 시각 센서
#include "Perception/AISenseConfig_Hearing.h" // 청각 센서
#include "Perception/AISenseConfig_Damage.h" // 데미지 센서
#include "BehaviorTree/BlackboardComponent.h" // 블랙보드 사용
#include "Kismet/KismetMathLibrary.h" // RInterpTo 사용을 위해 추가
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"


AAIC_Monster::AAIC_Monster()
{
    // 1. Perception Component 생성
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    // 2. 시각(Sight) 설정
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        SightConfig->SightRadius = 3000.0f;       // 시야 반경
        SightConfig->LoseSightRadius = 3100.0f;  // 시야를 잃는 반경 (더 크게 설정해야 안정적)
        SightConfig->PeripheralVisionAngleDegrees = 75.0f; // 시야각
        SightConfig->SetMaxAge(5.0f); // 감지 정보를 5초간 기억함

        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

        AIPerceptionComponent->ConfigureSense(*SightConfig);

    }

    // 3. 청각(Hearing) 설정
    UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    if (HearingConfig)
    {
        HearingConfig->HearingRange = 1500.0f;      // 청력 반경
        HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
        HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
        HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
        AIPerceptionComponent->ConfigureSense(*HearingConfig);
    }

    // 4. 촉각(Damage) 설정
    UAISenseConfig_Damage* DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
    if (DamageConfig)
    {
        AIPerceptionComponent->ConfigureSense(*DamageConfig);
    }

    // 5. 주된 감각을 시각으로 설정
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    // 6. 인지 정보 업데이트 함수 바인딩
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIC_Monster::OnTargetPerceptionUpdated);
}

void AAIC_Monster::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // 빙의 시 행동 트리 실행 (기존 로직)
}

// 6. 인지 정보 업데이트 시 실행될 함수의 내용 구현
void AAIC_Monster::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // 컨트롤하는 폰이나 블랙보드가 유효하지 않으면 아무것도 하지 않음
    if (GetPawn() == nullptr || GetBlackboardComponent() == nullptr)
    {
        return;
    }

    const FAISenseID SenseID = Stimulus.Type;
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
        UE_LOG(LogTemp, Warning, TEXT("Perception: HEARING stimulus received from %s"), *Actor->GetName());
    }

    // 감지된 액터를 '플레이어 캐릭터 클래스'로 캐스팅 시도
    ACharacter* TargetCharacter = Cast<ACharacter>(Actor);
    UBlackboardComponent* MyBlackboard = GetBlackboardComponent();

    if (Stimulus.WasSuccessfullySensed())
    {
        // 1. 감지된 Actor가 Pawn 타입인지 확인
        APawn* DetectedPawn = Cast<APawn>(Actor);
        if (DetectedPawn)
        {
            // 2. 해당 Pawn의 컨트롤러가 PlayerController인지 확인
            if (DetectedPawn->GetController() && DetectedPawn->GetController()->IsA<APlayerController>())
            {
                // 3. 플레이어가 조종하는 Pawn이 확실하므로, 타겟으로 설정
                MyBlackboard->SetValueAsObject(TEXT("TargetActor"), Actor);
                MyBlackboard->SetValueAsVector(TEXT("LastKnownLocation"), Actor->GetActorLocation());
            }
        }
    }
    else
    {
        // [감지 실패 - 시야에서 놓침]
        AActor* CurrentTarget = Cast<AActor>(MyBlackboard->GetValueAsObject(TEXT("TargetActor")));
        if (CurrentTarget == Actor)
        {
            MyBlackboard->ClearValue(TEXT("TargetActor"));
        }
    }
}

// Tick 함수 구현
void AAIC_Monster::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // bIsRotatingToTarget 플래그가 true일 때만 회전 로직 실행
    if (bIsRotatingToTarget)
    {
        // 1. 목표물과 AI 자신(Pawn)을 가져옴
        ACharacter* ControlledPawn = Cast<ACharacter>(GetPawn());
        AActor* TargetActor = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));

        if (ControlledPawn && TargetActor)
        {
            // 2. 목표물을 향하는 방향 벡터 계산
            FVector LookVector = TargetActor->GetActorLocation() - ControlledPawn->GetActorLocation();
            LookVector.Z = 0.f; // 수직 축은 무시

            // 3. 목표 회전값(Rotator) 계산
            FRotator TargetRotation = LookVector.Rotation();

            // 4. 부드러운 회전을 위해 보간(Interpolation) 사용
            // RInterpTo(현재 회전값, 목표 회전값, 경과 시간, 회전 속도)
            FRotator NewRotation = FMath::RInterpTo(ControlledPawn->GetActorRotation(), TargetRotation, DeltaSeconds, 5.0f);

            // 5. Pawn의 회전값을 직접 설정하는 대신, 컨트롤러의 회전값을 설정
            // 이렇게 하면 Pawn의 bUseControllerRotationYaw 설정에 따라 Pawn이 부드럽게 따라 회전함
            SetControlRotation(NewRotation);
        }
    }
}

// 회전 시작 함수 구현
void AAIC_Monster::StartRotatingToTarget()
{
    bIsRotatingToTarget = true;

    // 컨트롤러의 회전을 Pawn이 따라오도록 설정
    if (ACharacter* ControlledPawn = Cast<ACharacter>(GetPawn()))
    {
        ControlledPawn->GetCharacterMovement()->bOrientRotationToMovement = false;
        ControlledPawn->GetCharacterMovement()->bUseControllerDesiredRotation = true;
        ControlledPawn->bUseControllerRotationYaw = true;
    }
}

// 회전 종료 함수 구현
void AAIC_Monster::StopRotatingToTarget()
{
    bIsRotatingToTarget = false;

    // Pawn이 다시 애니메이션이나 다른 로직에 의해 회전하도록 설정을 되돌림
    if (ACharacter* ControlledPawn = Cast<ACharacter>(GetPawn()))
    {
        ControlledPawn->GetCharacterMovement()->bOrientRotationToMovement = true;
        ControlledPawn->GetCharacterMovement()->bUseControllerDesiredRotation = false;
        ControlledPawn->bUseControllerRotationYaw = false;
    }
}
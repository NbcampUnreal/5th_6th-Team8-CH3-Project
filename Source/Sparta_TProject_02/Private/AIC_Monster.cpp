// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Monster.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h" // 블랙보드 사용을 위해 추가
#include "Sparta_TProject_02/Sparta_TProject_02Character.h"

AAIC_Monster::AAIC_Monster()
{
    // 1. Perception Component 생성
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    // 2. 시각 감각(Sight) 설정 객체 생성
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        // 3. 시각 감각의 상세 설정
        SightConfig->SightRadius = 1500.0f;       // 시야 반경
        SightConfig->LoseSightRadius = 1600.0f;  // 시야를 잃는 반경 (더 크게 설정해야 안정적)
        SightConfig->PeripheralVisionAngleDegrees = 75.0f; // 시야각
        SightConfig->SetMaxAge(5.0f); // 감지 정보를 5초간 기억함

        // 어떤 대상을 감지할지 설정 (적대적, 중립적, 우호적)
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

        // 4. 생성한 시각 설정을 Perception Component에 적용
        AIPerceptionComponent->ConfigureSense(*SightConfig);
        // 주된 감각을 시각으로 설정
        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    }

    // 5. 인지 정보가 업데이트될 때 호출할 함수를 바인딩(연결)
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIC_Monster::OnTargetPerceptionUpdated);
}

void AAIC_Monster::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // 빙의 시 행동 트리 실행 (기존 로직)
    // ... RunBehaviorTree ...
}

// 6. 인지 정보 업데이트 시 실행될 함수의 내용 구현
void AAIC_Monster::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // 컨트롤하는 폰이나 블랙보드가 유효하지 않으면 아무것도 하지 않음
    if (GetPawn() == nullptr || GetBlackboardComponent() == nullptr)
    {
        return;
    }

    // 감지된 액터를 '플레이어 캐릭터 클래스'로 캐스팅 시도
    ASparta_TProject_02Character* PlayerCharacter = Cast<ASparta_TProject_02Character>(Actor);

    UBlackboardComponent* MyBlackboard = GetBlackboardComponent();

    // Stimulus.WasSuccessfullySensed() -> true이면 감지 성공, false이면 감지 실패(시야에서 사라짐)
    if (Stimulus.WasSuccessfullySensed())
    {
        // [감지 성공]
        // 감지된 대상이 플레이어가 맞는지 확인
        if (PlayerCharacter != nullptr)
        {
            // 1. 블랙보드에 'TargetActor'를 현재 감지된 플레이어로 설정
            MyBlackboard->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);

            // 2. 플레이어의 현재 위치를 'LastKnownLocation'에 계속 업데이트
            MyBlackboard->SetValueAsVector(TEXT("LastKnownLocation"), PlayerCharacter->GetActorLocation());
        }
    }
    else
    {
        // [감지 실패 - 시야에서 놓침]
        // 1. 현재 블랙보드에 등록된 타겟이 방금 놓친 그 액터인지 확인
        // (다른 액터를 추적 중에 엉뚱한 액터가 사라졌다는 신호를 무시하기 위함)
        AActor* CurrentTarget = Cast<AActor>(MyBlackboard->GetValueAsObject(TEXT("TargetActor")));
        if (CurrentTarget == Actor)
        {
            // 2. 'TargetActor' 키만 비워서 "더 이상 보이지 않는다"는 상태로 만듦
            MyBlackboard->ClearValue(TEXT("TargetActor"));
        }
    }
}
#pragma once

#include "CoreMinimal.h"
#include "AIMonsterBase.h"
#include "AIBoss.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API AAIBoss : public AAIMonsterBase
{
    GENERATED_BODY()

public:
    AAIBoss();

protected:
    virtual void BeginPlay() override;

public:
    // 부모의 Attack 함수를 재정의
    virtual void Attack() override;
};
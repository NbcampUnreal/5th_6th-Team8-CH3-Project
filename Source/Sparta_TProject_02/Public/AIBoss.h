// AIBoss.h

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
    // 게임 시작 시 한 번 호출됩니다.
    virtual void BeginPlay() override;

public:
    // 부모의 Attack 함수를 재정의
    virtual void Attack() override;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
    //class UAnimMontage* NormalAttackMontage;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
    //class UAnimMontage* SpecialAttackMontage;

private:
    // (개발 예정) 원래 방어력을 저장할 변수
    // float OriginalDefense; 
};
// AIZombie.h

#pragma once

#include "CoreMinimal.h"
#include "AIMonsterBase.h"
#include "AIZombie.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API AAIZombie : public AAIMonsterBase
{
    GENERATED_BODY()

public:
    AAIZombie();

    virtual void Attack() override;


    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Combat")
    //class UAnimMontage* AttackMontage;
};
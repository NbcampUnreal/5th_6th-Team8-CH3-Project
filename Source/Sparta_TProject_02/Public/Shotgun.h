#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "Shotgun.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API AShotgun : public AGunBase
{
	GENERATED_BODY()

public:
	AShotgun();

protected:
	// 샷건 펠릿이 퍼지는 최대 각도 (0~90) 
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float SpreadAngle;

	//한 번에 발사되는 펠릿(총알)의 수
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 PelletsPerShot;

	// 샷건의 다중 펠릿 로직을 위해 TraceFire를 재정의
	virtual void TraceFire() override;
};
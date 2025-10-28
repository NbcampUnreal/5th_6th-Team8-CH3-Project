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
	// ���� �縴�� ������ �ִ� ���� (0~90) 
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float SpreadAngle;

	//�� ���� �߻�Ǵ� �縴(�Ѿ�)�� ��
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 PelletsPerShot;

	// ������ ���� �縴 ������ ���� TraceFire�� ������
	virtual void TraceFire() override;
};
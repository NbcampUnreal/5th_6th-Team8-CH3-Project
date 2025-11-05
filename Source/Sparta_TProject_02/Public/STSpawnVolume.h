#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "STSpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class SPARTA_TPROJECT_02_API ASTSpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ASTSpawnVolume();

	//�ڽ� ���� �� ���� ��ġ ��ȯ
	UFUNCTION(BlueprintCallable, Category = "Spawn Volume")
	FVector GetRandomPointInVolume() const;
	//Ư�� ��ġ�� ���� �� �ִ���
	UFUNCTION(BlueprintCallable, Category = "Spawn Volume")
	bool IsPointInVolume(const FVector& Point) const;

	virtual void BeginPlay() override;

#if WITH_EDITOR
	//�����Ϳ��� ������Ƽ ���� �� ȣ�� 
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn Volume")
	TObjectPtr<UBoxComponent> SpawnBox;

	//�ӽ� ����� �ڽ� �ð�ȭ
	UPROPERTY(EditAnywhere, Category = "Spawn Volume|Debug")
	bool bShowDebugBox = true;
	UPROPERTY(EditAnywhere, Category = "Spawn Volume|Debug")
	FColor DebugBoxColor = FColor::Red;

	//z�� ������
	UPROPERTY(EditAnywhere, Category = "Spawn Volume")
	float ZOffset = 50.0f;

};

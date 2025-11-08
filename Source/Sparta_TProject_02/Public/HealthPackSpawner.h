#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPackSpawner.generated.h"

class AHealthPack;
UCLASS()
class SPARTA_TPROJECT_02_API AHealthPackSpawner : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* StaticMeshCompoent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TSubclassOf<AHealthPack> HealthPackClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	AHealthPack* SpawnedHealthPack;	// 스폰될 HealthPack
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	//USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float SpawnTime;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
public:	
	FTimerHandle SpawnTimerHandle;

	AHealthPackSpawner();
	void SpawnHealthPack();
};

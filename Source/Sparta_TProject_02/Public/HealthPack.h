#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPack.generated.h"

class USphereComponent;

UCLASS()
class SPARTA_TPROJECT_02_API AHealthPack : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* StaticMeshCompoent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	USphereComponent* SphereComponent;

	float BobbingStartpos;
	float BobbingHeight;

	virtual void BeginPlay() override;
public:
	FTimerManager BobbingTimerHandle;
	AHealthPack();
	void Bobbing();
};

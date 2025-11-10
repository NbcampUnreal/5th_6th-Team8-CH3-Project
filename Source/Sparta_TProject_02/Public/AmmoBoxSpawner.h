#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoBoxSpawner.generated.h"

class AAmmoBox;

UCLASS()
class SPARTA_TPROJECT_02_API AAmmoBoxSpawner : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* StaticMeshCompoent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TSubclassOf<AAmmoBox> AmmoBoxClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	AAmmoBox* SpawnedAmmoBox;	// ½ºÆùµÉ Ammo Box

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float SpawnTime;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
public:
	FTimerHandle SpawnTimerHandle;

	AAmmoBoxSpawner();
	void SpawnAmmoBox();

};

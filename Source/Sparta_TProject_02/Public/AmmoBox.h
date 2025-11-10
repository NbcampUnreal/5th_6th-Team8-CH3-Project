#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoBox.generated.h"

class USphereComponent;

UCLASS()
class SPARTA_TPROJECT_02_API AAmmoBox : public AActor
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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	FTimerHandle BobbingTimerHandle;
	AAmmoBox();
	void Bobbing();

	UFUNCTION()
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};

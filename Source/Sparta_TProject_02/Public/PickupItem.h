#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "PickupItem.generated.h"

class USphereComponent;
// class AItem;

UCLASS()
class SPARTA_TPROJECT_02_API APickupItem : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* StaticMeshCompoent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	USphereComponent* SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UItem* Item;
	//TSoftClassPtr<UItem*> Item;

	float BobbingStartpos;
	float BobbingHeight;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	FTimerHandle BobbingTimerHandle;

	APickupItem();
	virtual void Bobbing();

	UFUNCTION()
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};

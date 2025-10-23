#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class SPARTA_TPROJECT_02_API AItem : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* StaticMeshCompoent;

	int CurrentStack;
	int MaxStack;

	
	virtual void BeginPlay() override;
public:	
	AItem();
	virtual void Use();
};

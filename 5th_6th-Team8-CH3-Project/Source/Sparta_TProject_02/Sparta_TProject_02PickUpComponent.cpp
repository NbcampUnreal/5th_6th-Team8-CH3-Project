// Copyright Epic Games, Inc. All Rights Reserved.

#include "Sparta_TProject_02PickUpComponent.h"

USparta_TProject_02PickUpComponent::USparta_TProject_02PickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void USparta_TProject_02PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &USparta_TProject_02PickUpComponent::OnSphereBeginOverlap);
}

void USparta_TProject_02PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	ASparta_TProject_02Character* Character = Cast<ASparta_TProject_02Character>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}

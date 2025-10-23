#include "Item.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshCompoent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshCompoent->SetupAttachment(RootComponent);

	CurrentStack = 1;
	MaxStack = 1;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem::Use()
{

}
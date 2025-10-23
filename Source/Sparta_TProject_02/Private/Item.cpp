#include "Item.h"

AItem::AItem()
{
	StaticMeshCompoent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshCompoent->SetupAttachment(RootComponent);

 	PrimaryActorTick.bCanEverTick = false;
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
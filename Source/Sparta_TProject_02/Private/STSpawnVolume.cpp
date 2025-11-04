#include "STSpawnVolume.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASTSpawnVolume::ASTSpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->SetupAttachment(RootComponent);
	SpawnBox->SetBoxExtent(FVector(500.0f, 500.0f, 200.0f)); // �⺻ ũ��
	SpawnBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �浹 ��Ȱ��ȭ

	// �����Ϳ��� ���̵��� ����
	SpawnBox->SetHiddenInGame(true);
	SpawnBox->bVisualizeComponent = true;

}

void ASTSpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	// ���� ���� �� �α�
	UE_LOG(LogTemp, Log, TEXT("[SpawnVolume] '%s' Finished Initialized (transform: %s, scale: %s)"),
		*GetName(),
		*GetActorLocation().ToString(),
		*SpawnBox->GetScaledBoxExtent().ToString());
}

FVector ASTSpawnVolume::GetRandomPointInVolume() const
{
	if (!SpawnBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnVolume] SpawnBox�� ��ȿ���� �ʽ��ϴ�!"));
		return GetActorLocation();
	}

	// �ڽ��� ���� ��ġ, ũ��
	FVector Origin = SpawnBox->GetComponentLocation();
	FVector BoxExtent = SpawnBox->GetScaledBoxExtent();

	// �ڽ� ������ ���� ��ġ ���
	FVector RandomPoint;
	RandomPoint.X = Origin.X + FMath::RandRange(-BoxExtent.X, BoxExtent.X);
	RandomPoint.Y = Origin.Y + FMath::RandRange(-BoxExtent.Y, BoxExtent.Y);
	RandomPoint.Z = Origin.Z + ZOffset; // Z���� ���� (���� ��)

	return RandomPoint;

}

bool ASTSpawnVolume::IsPointInVolume(const FVector& Point) const
{
	if (!SpawnBox)
	{
		return false;
	}

	// �ڽ� ���� ���� �ִ��� Ȯ��
	FBox BoundingBox = SpawnBox->Bounds.GetBox();
	return BoundingBox.IsInside(Point);
}

#if WITH_EDITOR
void ASTSpawnVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// �����Ϳ��� ������Ƽ ���� �� �α�
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ASTSpawnVolume, bShowDebugBox) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ASTSpawnVolume, DebugBoxColor))
	{
		UE_LOG(LogTemp, Log, TEXT("[SpawnVolume] '%s' ����� ���� �����"), *GetName());
	}
}
#endif


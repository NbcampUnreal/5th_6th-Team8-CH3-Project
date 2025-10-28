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
	SpawnBox->SetBoxExtent(FVector(500.0f, 500.0f, 200.0f)); // 기본 크기 
	SpawnBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌 비활성화 

	// 에디터에서 보이도록 설정
	SpawnBox->SetHiddenInGame(true);
	SpawnBox->bVisualizeComponent = true;

}

virtual void BeginPlay() override
{
	Super::BeginPlay();

	// 게임 시작 시 로그
	UE_LOG(LogTemp, Log, TEXT("[SpawnVolume] '%s' Finished Initialized (transform: %s, scale: %s)"),
		*GetName(),
		*GetActorLocation().ToString(),
		*SpawnBox->GetScaledBoxExtent().ToString());
}

FVector ASTSpawnVolume::GetRandomPointInVolume() const
{
	if (!SpawnBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnVolume] SpawnBox가 유효하지 않습니다!"));
		return GetActorLocation();
	}

	// 박스의 월드 위치, 크기
	FVector Origin = SpawnBox->GetComponentLocation();
	FVector BoxExtent = SpawnBox->GetScaledBoxExtent();

	// 박스 내부의 랜덤 위치 계산
	FVector RandomPoint;
	RandomPoint.X = Origin.X + FMath::RandRange(-BoxExtent.X, BoxExtent.X);
	RandomPoint.Y = Origin.Y + FMath::RandRange(-BoxExtent.Y, BoxExtent.Y);
	RandomPoint.Z = Origin.Z + ZOffset; // Z축은 고정 (지면 위)

	return RandomPoint;

}

bool ASTSpawnVolume::IsPointInVolume(const FVector& Point) const
{
	if (!SpawnBox)
	{
		return false;
	}

	// 박스 영역 내에 있는지 확인
	FBox BoundingBox = SpawnBox->Bounds.GetBox();
	return BoundingBox.IsInside(Point);
}

#if WITH_EDITOR
void ASTSpawnVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 에디터에서 프로퍼티 변경 시 로그
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ASTSpawnVolume, bShowDebugBox) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ASTSpawnVolume, DebugBoxColor))
	{
		UE_LOG(LogTemp, Log, TEXT("[SpawnVolume] '%s' 디버그 설정 변경됨"), *GetName());
	}
}
#endif


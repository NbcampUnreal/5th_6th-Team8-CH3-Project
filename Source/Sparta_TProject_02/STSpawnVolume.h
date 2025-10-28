#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "STSpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class SPARTA_TPROJECT_02_API ASTSpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTSpawnVolume();

	//박스 볼륨 내 랜덤 위치 반환
	UFUNCTION(BlueprintCallable, Category = "Spawn Volume")
	FVector GetRandomPointInVolume() const;
	//특정 위치가 볼륨 내 있는지
	UFUNCTION(BlueprintCallable, Category = "Spawn Volume")
	bool IsPointInVolume(const FVector& Point) const;

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	/** 에디터에서 프로퍼티 변경 시 호출 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn Volume")
	TObjectPtr<UBoxComponent> SpawnBox;

	//임시 디버깅 박스 시각화
	UPROPERTY(EditAnywhere, Category = "Spawn Volume|Debug")
	bool bShowDebugBox = true;
	UPROPERTY(EditAnywhere, Category = "Spawn Volume|Debug")
	FColor DebugBoxColor = FColor::Red;

	//z축 오프셋
	UPROPERTY(EditAnywhere, Category = "Spawn Volume")
	float ZOffset = 50.0f;

};

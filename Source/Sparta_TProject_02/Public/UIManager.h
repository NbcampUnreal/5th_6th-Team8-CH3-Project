#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UIManager.generated.h"

class UMainMenuWidget;
class UGameHUDWidget;

UCLASS()
class SPARTA_TPROJECT_02_API UUIManager : public UObject
{
	GENERATED_BODY()
	
public:
    void Init(APlayerController* InPC);
    void ShowMainMenu();
    void ShowGameHUD();
    void UpdateHealth(float NewHP);
    void UpdateWaveInfo(int32 CurrentWave, float TimeToNext);

private:
    UPROPERTY()
    APlayerController* PC = nullptr;

    UPROPERTY()
    UUserWidget* CurrentWidget = nullptr;

    UPROPERTY()
    UMainMenuWidget* MainMenuWidget = nullptr;

    UPROPERTY()
    UGameHUDWidget* GameHUDWidget = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMainMenuWidget> MainMenuClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGameHUDWidget> GameHUDClass;

};

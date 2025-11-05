#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    //  버튼 이벤트 연결
    if (StartButton)
        StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleStartClicked);

    if (ExitButton)
        ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleExitClicked);

    //  메뉴 진입 시 UI 전용 입력 모드
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(TakeWidget());
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

void UMainMenuWidget::HandleStartClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked!"));

    //  이동할 맵 경로 (정확한 경로로 지정)
    FString LevelPath = TEXT("/Game/00_Levels/ZombieWave_Main");

    //  GameMode 명시적으로 지정 (선택사항이지만 권장)
    FString Options = TEXT("?game=/Game/Blueprints/GameMode/STGameMode.STGameMode_C");

    //  Input 모드 복구 (UIOnly → GameOnly)
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }

    //  맵 전환 실행
    UGameplayStatics::OpenLevel(this, FName(*LevelPath), true, Options);
}

void UMainMenuWidget::HandleExitClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Exit Button Clicked!"));

    //  정상적인 게임 종료
    if (APlayerController* PC = GetOwningPlayer())
    {
        UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
    }
}

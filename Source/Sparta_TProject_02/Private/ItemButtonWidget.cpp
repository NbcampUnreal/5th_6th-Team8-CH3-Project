#include "ItemButtonWidget.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "MyGameInstance.h"

void UItemButtonWidget::NativeConstruct()
{
   Super::NativeConstruct();
}

UItemButtonWidget::UItemButtonWidget(const FObjectInitializer& ObjectInitializer)
   : Super(ObjectInitializer)
{
  // Button = ObjectInitializer.CreateDefaultSubobject<UButton>(this, TEXT("MainButton"));
}

void UItemButtonWidget::BeginDestroy()
{
   Super::BeginDestroy();
   UE_LOG(LogTemp, Warning, TEXT("UItemButtonWidget GC complted"));
   GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("UItemButtonWidget GC complted"));
}

void UItemButtonWidget::HandleButtonHovered()
{
   OnHovered.Broadcast(ButtonData);
}

void UItemButtonWidget::HandleButtonClicked()
{
   OnClicked.Broadcast(ButtonData);
}

UButton* UItemButtonWidget::GetButton() const
{
   return MainButton;
}

FItemButtonData UItemButtonWidget::GetButtonData() const
{
   return ButtonData;
}

void UItemButtonWidget::SetButton(UButton* NewButton)
{
   MainButton = NewButton;
}

void UItemButtonWidget::SetButtonData(FItemButtonData NewButtonData)
{
   ButtonData = NewButtonData;
}

void UItemButtonWidget::SetupHoverBinding()
{
   if (MainButton)
   {
      MainButton->OnHovered.AddDynamic(this, &UItemButtonWidget::HandleButtonHovered);
   }
}
void UItemButtonWidget::SetupClickBinding()
{
   if (MainButton)
   {
      MainButton->OnClicked.AddDynamic(this, &UItemButtonWidget::HandleButtonClicked);
   }
}

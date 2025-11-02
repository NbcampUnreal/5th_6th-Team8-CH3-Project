#include "ItemButtonWidget.h"
#include "Components/Button.h"
#include "MyGameInstance.h"

void UItemButtonWidget::NativeConstruct()
{
   Super::NativeConstruct();
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
   return Button;
}

FItemButtonData UItemButtonWidget::GetButtonData() const
{
   return ButtonData;
}

void UItemButtonWidget::SetButton(UButton* NewButton)
{
   Button = NewButton;
}

void UItemButtonWidget::SetButtonData(FItemButtonData NewButtonData)
{
   ButtonData = NewButtonData;
}

void UItemButtonWidget::SetupHoverBinding()
{
   if (Button)
   {
      Button->OnHovered.AddDynamic(this, &UItemButtonWidget::HandleButtonHovered);
   }
}
void UItemButtonWidget::SetupClickBinding()
{
   if (Button)
   {
      Button->OnClicked.AddDynamic(this, &UItemButtonWidget::HandleButtonClicked);
   }
}

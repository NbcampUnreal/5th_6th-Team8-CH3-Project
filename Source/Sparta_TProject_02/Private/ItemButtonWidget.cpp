#include "ItemButtonWidget.h"
#include "Components/Button.h"
#include "MyGameInstance.h"

void UItemButtonWidget::NativeConstruct()
{
   Super::NativeConstruct();
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

void UItemButtonWidget::ButtonBinding()
{
   if (Button)
   {
      Button->OnHovered.AddDynamic(this, &UItemButtonWidget::HandleButtonHovered);
   }
}
void UItemButtonWidget::HandleButtonHovered()
{
   OnHovered.Broadcast(ButtonData);
}

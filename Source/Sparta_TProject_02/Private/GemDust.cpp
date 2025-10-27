#include "GemDust.h"

UGemDust::UGemDust()
{
	ItemName = "Gem Dust";
	ItemPrice = 10;
	bIsStackable = true;
	CurrentStack = 1;
	MaxStack = 60;
}
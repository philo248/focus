// Copyright BlindGames. All Rights Reserved.

#include "Viewfinder.h"
#include "Components/Image.h"

void UViewfinder::ToggleFlashIconVisibility(bool FlashStatus)
{
	if (FlashStatus)
	{
		FlashIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FlashIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

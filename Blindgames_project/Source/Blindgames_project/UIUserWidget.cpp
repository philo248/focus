// Copyright BlindGames. All Rights Reserved.


#include "UIUserWidget.h"
#include "InGameHUD.h"

void UUIUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Player HUD reference
	GameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

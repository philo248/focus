// Copyright BlindGames. All Rights Reserved.

/*
 * Contain the fade in/out animation when the camera is turn on or off.
 * The animation is played inside a widget.
 */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Viewfinder.generated.h"

/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API UViewfinder : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	class UWidgetAnimation* FadeAnim;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "FlashIcon")
	class UImage* FlashIcon;
public:

	UFUNCTION()
	void ToggleFlashIconVisibility(bool FlashStatus);
};

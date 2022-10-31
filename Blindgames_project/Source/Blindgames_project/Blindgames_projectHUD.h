// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blindgames_projectHUD.generated.h"

UCLASS()
class ABlindgames_projectHUD : public AHUD
{
	GENERATED_BODY()

public:
	ABlindgames_projectHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};


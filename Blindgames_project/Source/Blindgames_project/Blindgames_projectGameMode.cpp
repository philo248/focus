// Copyright Epic Games, Inc. All Rights Reserved.

#include "Blindgames_projectGameMode.h"
#include "Blindgames_projectHUD.h"
#include "Blindgames_projectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABlindgames_projectGameMode::ABlindgames_projectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ABlindgames_projectHUD::StaticClass();
}

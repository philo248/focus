// Copyright BlindGames. All Rights Reserved.

#include "PhotoGalleryGameInstance.h"
#include "PhotoGallery.h"
#include "Kismet/GameplayStatics.h"

const FString UPhotoGalleryGameInstance::UNIQUE_SAVE_SLOT = "SaveGameSlot_01"; // This is the unique name used for the game save

bool UPhotoGalleryGameInstance::IsNewGame()
{
	if (UGameplayStatics::DoesSaveGameExist(UNIQUE_SAVE_SLOT, 1)) // check if the game save exist
	{
		return  true;
	}
	return false;
}

bool UPhotoGalleryGameInstance::CreateNewSaveGame()
{
	if (CurrentSaveGame == nullptr) // the game save doesn't exist
	{
		USaveGame* NewSaveGame = UGameplayStatics::CreateSaveGameObject(UPhotoGallery::StaticClass());

		if (NewSaveGame != nullptr)
		{
			CurrentSaveGame = Cast<UPhotoGallery>(NewSaveGame);
			
			if (CurrentSaveGame)
			{
				CurrentSaveGame->CreateSlot(UNIQUE_SAVE_SLOT); // Create new game save with the unique name
			}
		}
	}
	else // The game save exist
	{
		CurrentSaveGame->CreateSlot(UNIQUE_SAVE_SLOT); // Create new game save with the unique name
	}

	return UGameplayStatics::SaveGameToSlot(CurrentSaveGame, UNIQUE_SAVE_SLOT, 1);
}

bool UPhotoGalleryGameInstance::LoadGame()
{
	CurrentSaveGame = nullptr;

	USaveGame* Slot = UGameplayStatics::LoadGameFromSlot(UNIQUE_SAVE_SLOT, 1); // Load the game save with a specific name

	if (Slot != nullptr)
	{
		CurrentSaveGame = Cast<UPhotoGallery>(Slot);

		if (CurrentSaveGame != nullptr)
		{
			return true;
		}
	}
	return false;
}

bool UPhotoGalleryGameInstance::SaveGame()
{
	if (CurrentSaveGame != nullptr)
	{
		return UGameplayStatics::SaveGameToSlot(CurrentSaveGame, UNIQUE_SAVE_SLOT, 1); // save the game
	}

	return false;
}

bool UPhotoGalleryGameInstance::DeleteGame()
{
	if (CurrentSaveGame != nullptr)
	{
		CurrentSaveGame = nullptr;
		return UGameplayStatics::DeleteGameInSlot(UNIQUE_SAVE_SLOT, 1);
	}

	return false;
}

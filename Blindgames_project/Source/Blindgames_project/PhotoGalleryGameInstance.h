// Copyright BlindGames. All Rights Reserved.

/*
* Game instance for the game.
* The game instance contain all the function to create, load, save the game save
* and check if a game save exist.
*/

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PhotoGalleryGameInstance.generated.h"

class UQuestData;
class UPhotoGallery;

/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API UPhotoGalleryGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	static const FString UNIQUE_SAVE_SLOT; // Unique name for the game save

public:

	UPROPERTY()
	UPhotoGallery* CurrentSaveGame; // Reference of the actual game save
	
public:
	
	bool IsNewGame(); // Check if a save exist, return if a save exist or not

	bool CreateNewSaveGame(); // Create a new save game, return if it succeed

	bool LoadGame(); // Load an existing save game, return true or false if the game is load or not

	bool SaveGame(); // Save the game, return if it succeed

	bool DeleteGame(); // Delete the game save, return if game save has been successfully deleted
};

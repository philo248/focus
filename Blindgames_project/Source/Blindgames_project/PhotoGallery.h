// Copyright BlindGames. All Rights Reserved.

/*
* The actual game save.
* The game save keep in memory the number of picture in the photo gallery.
* It also contain a reference for all pictures inside an array.
* The game save have a unique name.
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PhotoGallery.generated.h"

/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API UPhotoGallery : public USaveGame
{
	GENERATED_BODY()

public:

	// Name of the actual game save
	UPROPERTY(EditAnywhere, Category = "Save")
	FString SaveGameName;
	
	// Number of picture in the gallery
	UPROPERTY(EditAnywhere, Category = "PictureCount")
	int32 PictureCount;


	// Number of picture in the gallery
	UPROPERTY(EditAnywhere, Category = "PictureCount")
	int32 asscount;
	
	// Picture array
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Photo")
	TArray<int32> PictureNumbers;

	// Dialogue text array
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Photo")
	TArray<FText> DialogueTextID; // Dialogue text ID. If value = None, then no dialogue is link with this photo
	
public:

	// Simple function to name the game save
	void CreateSlot(const FString& SlotName)
	{
		SaveGameName = SlotName;
		PictureCount = 0;
		asscount = 0;
		PictureNumbers.Empty();
		DialogueTextID.Empty();
	}
	
	void RemovePicture(int32 PictureNumber)
	{
		DialogueTextID.RemoveAt(PictureNumbers.Find(PictureNumber));
		PictureNumbers.Remove(PictureNumber);		
	}
	
};

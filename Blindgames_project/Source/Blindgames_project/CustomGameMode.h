// Copyright BlindGames. All Rights Reserved.

/*
* The custom game mode for the game.
* Used to set the game on pause and set quest status
* Function are created in this class to set and unset quests status
*/

#pragma once

#include "CoreMinimal.h"
#include "QuestData.h"
#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

class UQuestData;
/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	void SetGamePaused(bool GameIsPause);

	void SetFoundQuest(int QuestNumb){ Quest->QuestDataList[QuestNumb].IsFound = true; }; // Set quest status to found
	void UnsetFoundQuest(int QuestNumb){ Quest->QuestDataList[QuestNumb].IsFound = false; }; // Set quest status to not found
	void CompletQuest(int QuestNumb){ Quest->QuestDataList[QuestNumb].IsCompleted = true; }; // Set quest status to completed

	bool CheckIfPhotoIsValid(int32 SelectedPictureID); // Check if a specific photo is valid and exist inside a quest among the three, need to send a picture number. Return true or false if the photo is valid or not
	bool CheckIfPhotoIsInQuest(int32 SelectedPictureID, int32 QuestID); // Check if a photo is valid for a specific quest, need to send a picture number and a quest number, Return true or false if the photo is valid for this quest
	bool CheckIfQuestContainsPhoto(int32 QuestID); // Check if a quest contain valid photo, need to send a quest number, Return true or false if the quest contain valid photo
	bool CheckIfAllQuestAreCompleted(); // Validate if all quests are completed. Return true if all quests are completed, or false if not.

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	UQuestData* Quest; // Quest, contains all information about quests and their completion

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

// Copyright BlindGames. All Rights Reserved.

/*
* Container array for all the quest data in the game
* Use to store the quest ID, distortion tag, pictures number reference,
* If a photo is found for a specific quest,
* If a quest is complete
*/

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestData.generated.h"

USTRUCT(BlueprintType)
struct FQuest
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName QuestID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName DistortionActorTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TArray<int32> PicturesNumbRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	bool IsFound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	bool IsCompleted;
};

/**
 * 
 */
UCLASS(BlueprintType)
class BLINDGAMES_PROJECT_API UQuestData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TArray<FQuest> QuestDataList;
};

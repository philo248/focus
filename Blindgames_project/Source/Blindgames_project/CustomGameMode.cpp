// Copyright BlindGames. All Rights Reserved.

#include "CustomGameMode.h"
#include "QuestData.h"

void ACustomGameMode::SetGamePaused(bool GameIsPause)
{
    APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
    if (MyPlayer != NULL)
    {
        MyPlayer->SetPause(GameIsPause);
    }
}

void ACustomGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    for (int i = 0; i < Quest->QuestDataList.Num(); i++)
    {
        Quest->QuestDataList[i].IsCompleted = false;
        Quest->QuestDataList[i].PicturesNumbRef.Empty();
    }
}

bool ACustomGameMode::CheckIfPhotoIsValid(int32 SelectedPictureID)
{
    for (int32 i = 0; i < Quest->QuestDataList.Num(); i++)
    {
        TArray<int32> PhotoIDList = Quest->QuestDataList[i].PicturesNumbRef;

        for (int32 PhotoIDNumber : PhotoIDList)
        {
            if (PhotoIDNumber == SelectedPictureID)
            {
                return true;
            }
        }
    }
    
    return false;
}

bool ACustomGameMode::CheckIfPhotoIsInQuest(int32 SelectedPictureID, int32 QuestID)
{
    TArray<int32> PhotoIDList = Quest->QuestDataList[QuestID].PicturesNumbRef;

    for (int32 PhotoIDNumber : PhotoIDList)
    {
        if (PhotoIDNumber == SelectedPictureID && Quest->QuestDataList[QuestID].IsFound)
        {
            return true;
        }
    }
    
    return false;
}

bool ACustomGameMode::CheckIfQuestContainsPhoto(int32 QuestID)
{
    int32 numbPhotoInQuest = Quest->QuestDataList[QuestID].PicturesNumbRef.Num();
		
    if (numbPhotoInQuest > 0)
    {
        return true;
    }
    
    return false;
}

bool ACustomGameMode::CheckIfAllQuestAreCompleted()
{
    for (int32 i = 0; i < Quest->QuestDataList.Num(); i++)
    {
        if (!Quest->QuestDataList[i].IsCompleted)
        {
            return false;
        }
    }

    return true;
}



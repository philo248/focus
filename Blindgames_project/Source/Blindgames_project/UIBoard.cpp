// Copyright BlindGames. All Rights Reserved.

#include "UIBoard.h"
#include "CustomGameMode.h"
#include "InGameHUD.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UUIBoard::Initialize()
{
	Super::Initialize();

	//Delegate button functions binding
	SituationBtn->OnClicked.AddDynamic(this, &UUIBoard::SituationBtnClicked);
	EmotionBtn->OnClicked.AddDynamic(this, &UUIBoard::EmotionBtnClicked);
	PenseeBtn->OnClicked.AddDynamic(this, &UUIBoard::PenseeBtnClicked);
	QuitBtn->OnClicked.AddDynamic(this, &UUIBoard::QuitBtnClicked);
	
	return true;
}

void UUIBoard::NativeConstruct()
{
	Super::NativeConstruct();

	// Call parent class construct
	UUIUserWidget::NativeConstruct();

	//Set sorting mode to true
	if (GameHUD)
	{
		GameHUD->isSorting = true;
	}

	ACustomGameMode* GameMode = Cast<ACustomGameMode>(GetWorld()->GetAuthGameMode());

	// Enable all button for each quest not complete
	for(int i = 0; i < GameMode->Quest->QuestDataList.Num(); i++)
	{
		if (GameMode->Quest->QuestDataList[i].IsCompleted)
		{
			DisableButton(i);
		}
	}

	// Enable distortion button if all the quest are complete
	if (GameMode->CheckIfAllQuestAreCompleted())
	{
		EnableDistortionButton();
	}
	else
	{
		DisableButton(3); // Disable the distortion button if not all quest are completed
	}
}

void UUIBoard::SituationBtnClicked()
{
	if (GameHUD)
	{
		if (ValidateSound)
		{
			UGameplayStatics::PlaySound2D(this, ValidateSound);
		}
		
		GameHUD->QuestButtonID = 0;
		GameHUD->ToggleGallery();
	}
}

void UUIBoard::EmotionBtnClicked()
{
	if (GameHUD)
	{
		if (ValidateSound)
		{
			UGameplayStatics::PlaySound2D(this, ValidateSound);
		}
		
		GameHUD->QuestButtonID = 1;
		GameHUD->ToggleGallery();
	}
}

void UUIBoard::PenseeBtnClicked()
{
	if (GameHUD)
	{
		if (ValidateSound)
		{
			UGameplayStatics::PlaySound2D(this, ValidateSound);
		}
		
		GameHUD->QuestButtonID = 2;
		GameHUD->ToggleGallery();
	}
}

void UUIBoard::QuitBtnClicked()
{
	//Set sorting mode to false
	if (GameHUD)
	{
		GameHUD->isSorting = false;
	}

	//Remove all widget from viewport
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

	// Set input mode to Game
	FInputModeGameOnly InputModeGame;
	GameHUD->PlayerControllerRef->SetInputMode(InputModeGame);
	GameHUD->PlayerControllerRef->ResetIgnoreInputFlags();

	//Restart Message to access the board
	GameHUD->AccessBoardView();
}

void UUIBoard::DisableButton(int32 ButtonQuestID)
{
	switch (ButtonQuestID) // Disable a button based on the button ID
	{
	case 0: SituationBtn->SetIsEnabled(false);
		break;
	case 1: EmotionBtn->SetIsEnabled(false);
		break;
	case 2: PenseeBtn->SetIsEnabled(false);
		break;
	case 3: DistortionBtn->SetIsEnabled(false);
		break;
	default:
		break;
	}
}

void UUIBoard::EnableDistortionButton()
{
	DistortionBtn->SetIsEnabled(true);
}

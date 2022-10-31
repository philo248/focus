// Copyright BlindGames. All Rights Reserved.

/*
* UI class for the sorting board.
* Contain all the function when a player clicked on a button in the UI
* It also keep the sound when a button change focus.
*/

#pragma once

#include "CoreMinimal.h"
#include "UIUserWidget.h"
#include "UIBoard.generated.h"

/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API UUIBoard : public UUIUserWidget
{
	GENERATED_BODY()
public:
	
	virtual bool Initialize();

	virtual void NativeConstruct() override;

	/******* Buttons for each category *******/
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UButton* EmotionBtn;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UButton* PenseeBtn;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UButton* SituationBtn;
	
	// Button to choose the distortion
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UButton* DistortionBtn;

	//Button to quit the board UI
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UButton* QuitBtn;

	// Click sound when changing button focus
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USoundWave* ValidateSound;
	
public:

	/******* Function when a button is clicked ******/
	
	UFUNCTION()
	void EmotionBtnClicked();

	UFUNCTION()
	void PenseeBtnClicked();

	UFUNCTION()
	void SituationBtnClicked();

	UFUNCTION()
	void QuitBtnClicked();

	// Use to disable a button when a quest is completed
	UFUNCTION()
	void DisableButton(int32 ButtonQuestID);

	// When the quest is done, enable the distortion button to allow player to choose the distortion
	UFUNCTION()
	void EnableDistortionButton();
};

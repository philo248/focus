// Copyright BlindGames. All Rights Reserved.

/*
* The game HUD for the player.
* Contain all the function to add or remove a specific widget for the player.
* It also keep the sounds for all the widget.
*/

#pragma once

#include "CoreMinimal.h"
#include "UIDialogue.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

/**
 * 
 */
UCLASS()

class BLINDGAMES_PROJECT_API AInGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	AInGameHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void StartGameHUD(); // Start the game and show BaseView
	
	UFUNCTION()
	void EnableCameraView(); // Show the camera view finder widget

	UFUNCTION()
	void DisableCameraView(); // Hide the camera view finder widget

	UFUNCTION()
	void ToggleFlashIcon(bool FlashStatus); // Toggle the flash icon in the camera view when player activate the flash
	
	UFUNCTION()
	void ToggleGallery(); // Show/Hide the photo gallery

	UFUNCTION()
	void NextPage(); // Go to next page in the gallery

	UFUNCTION()
	void PrevPage();// Go to prev page in the gallery

	UFUNCTION()
	void NextPhoto(); // Go to next photo in the gallery

	UFUNCTION()
	void TopPhoto(); // Go to Top photo on the gallery

	UFUNCTION()
	void BottomPhoto();// Go to Bottom photo on the gallery

	UFUNCTION()
	void PrevPhoto();// Go to prev photo in the gallery

	UFUNCTION()
	void DelPhoto();// Delete the selected photo on the gallery

	UFUNCTION()
	bool ToggleZoomPhoto();// Zoom the selected photo on the gallery,return true if the photo is successfully zoom in or out

	UFUNCTION()
	bool ValidateQuestMessage(); // Show success message to player if the selected photo is valid, start logics in other classes to complete the quest and return true. if it's not valid, show error message to player and return false.

	UFUNCTION()
	void ShowMenuPause(); // Show menu pause

	UFUNCTION(BlueprintCallable)
	void AccessBoardView(); // Show Access Board View

	UFUNCTION(BlueprintCallable)
	void DisplayBoardView(); // Show Access Board View

	UFUNCTION(BlueprintCallable)
	void DisplayDistortionCognitiveView(); // Show Access Board View

	UFUNCTION()
	void EndAccessBoardView(); // Show Access Board View

	UFUNCTION()
	FText GetDialogue(); // Function to get the text inside the dialogue UI
	
	UFUNCTION(BlueprintCallable)
	void ShowDialogue(FName LineID); // Function to show dialogue UI on the screen of the player
	
	UFUNCTION()
	bool GetKeepGalleryOpen() const; // Getter to see if the gallery must stay open
	
	UFUNCTION()
	void SetKeepGalleryOpen(bool bKeepGalleryOpen); // Setter keep the the gallery open
	
	UPROPERTY(VisibleAnywhere, Category = "Player")
	APlayerController* PlayerControllerRef; // Player controller reference
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Sorting")
	bool isSorting; // Indicate if the player is sorting photo on the board

	UPROPERTY(VisibleAnywhere, Category = "Sorting")
	int32 QuestButtonID; // Stock the selected button id on the board widget

	UPROPERTY(EditAnywhere, Category = "Gallery")
	bool IsInGalleryMode; // Check if the gallery is on or off
	
	UPROPERTY(VisibleAnywhere, Category = "Gallery")
	bool isPhotoFullscreen; // Indicate if a photo is full screen in the gallery

	/***** Widget classes reference set in blueprint *****/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> AccessBoardClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> BaseViewClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> CameraViewClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> PhotoGalleryClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> PhotoFullscreen;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> PauseMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> DialogueClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> DialogueBoardClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> DisplayBoardClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> FirstTimeDisplayBoardClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> SelectDistortionCognitiveClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Board")
	TSubclassOf<class ABoard> BoardClass;

	/********************************/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	class UUserWidget* CurrentWidget; // The current widget displayed on screen

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	class UUserWidget* OverlayWidget; // The second widget displayed on screen
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "GallerySort")
	bool FirstTimeBoard; // Indicate if the player use the board for the first time
	
	UPROPERTY(EditAnyWhere, Category = "GallerySort")
	int32 SelectedPicture; // Indicate the number of the photo selected in the gallery

	UPROPERTY(EditAnyWhere, Category = "GallerySort")
	UUIDialogue* DialogueUI; // reference to the dialogue UI widget class

	/***** Sounds for UI *****/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USoundWave* CloseBookSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USoundWave* OpenBookSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USoundWave* TurnPageSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USoundWave* GoodSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USoundWave* BadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USoundWave* ZoomSound;

private:

	UPROPERTY()
	bool KeepGalleryOpen; // Used to keep the gallery open when player quit fullscreen photo

};

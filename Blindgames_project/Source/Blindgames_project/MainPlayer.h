// Copyright BlindGames. All Rights Reserved.

/*
 * This is the main player of the game.
 * The player have a photo camera and he
 * can use it to shoot picture.
 * This is where all the player's inputs are received.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"

class AInGameHUD;
class UCameraComponent;

UCLASS()
class BLINDGAMES_PROJECT_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AMainPlayer();

	// GameHUD reference for the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	AInGameHUD* GameHUD;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;
	
	// Camera component reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UChildActorComponent* CameraComponent;

	// Camera component reference
	UPROPERTY(EditAnywhere, Category = "Component")
	class AInstantCameraComponent* CameraRef;
	
protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	void EnableCameraControl(); // Enable the camera and the gallery after the player pass the tutorial at the begining of the game
	
	UFUNCTION()
	void CameraAction(); // Take out or put away the camera

	UFUNCTION()
	void Cancel(); // put away 
	
	UFUNCTION()
	void TakePicture(); // Take out or put away the camera

	UFUNCTION()
	void NextPage(); // Go to next page on the gallery

	UFUNCTION()
	void PrevPage();// Go to prev page on the gallery

	UFUNCTION()
	void NextPhoto(); // Go to next photo on the gallery

	UFUNCTION()
	void PrevPhoto();// Go to prev photo on the gallery

	UFUNCTION()
	void TopPhoto(); // Go to Top photo on the gallery

	UFUNCTION()
	void BottomPhoto();// Go to Bottom photo on the gallery
	
	UFUNCTION()
	void DelPhoto();// Delete the selected photo on the gallery

	UFUNCTION()
	void ZoomPhoto();// Zoom the selected photo on the gallery

	UFUNCTION()
	void DisplayBoard();// Display board
	
	UFUNCTION()
	void ToggleMenuPause(); // Open menu pause

	UFUNCTION()
	void ToggleCameraFlash(); // Toggle the camera flash by the player
	
	// Getter and setter for all variable of the player
	UFUNCTION()
	void SetIsInPhotoMode(bool CurrentStatus){ IsInPhotoMode = CurrentStatus; } // Use to tell that the camera is on
	UFUNCTION()
	bool GetIsInPhotoMode(){ return IsInPhotoMode; } // Check if the player is in photo mode
	UFUNCTION()
	void SetCanOpenBoard(bool CurrentStatus){ CanOpenBoard = CurrentStatus; } // Use to tell that the player is near the sorting board
	UFUNCTION()
	bool GetCanOpenBoard(){ return CanOpenBoard; } //Check if the player is near the sorting board and he can access it

protected:
	UFUNCTION()
    void ToggleGallery(); // Hide camera

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UPROPERTY(EditAnywhere, Category = "Camera")
	bool IsInPhotoMode; // Check if the camera is on or off

	UPROPERTY(EditAnyWhere, Category = "GallerySort")
	bool CanOpenBoard; // If player can open the sorting board in the office

	UPROPERTY()
	class UInputComponent* MainPlayerInputComponent; // Store all player input actions
};

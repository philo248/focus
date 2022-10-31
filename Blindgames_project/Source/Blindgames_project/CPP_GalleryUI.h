// Copyright BlindGames. All Rights Reserved.

/*
* This Class contains the methods and variables
* Used within The Gallery UI.
*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"
#include "CPP_GalleryUI.generated.h"

class UCPP_UIInstantPhoto;
class UPhotoGalleryGameInstance;
/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API UCPP_GalleryUI : public UUserWidget
{
	GENERATED_BODY()
	
	virtual bool Initialize();


public:
	/**
	* Get the texture of the photo.
	*
	* This fonction get the image of the picture and apply it to a 2D texture
	* and send it back.
	*
	* @param FullFilePath Value that contains the path to the picture.
	* @return the picture as a texture.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Import Image", CompactNodeTitle = "IMG Import", Keywords = "import image texture"), Category = "Instant Camera")
	static UTexture2D* GetTexture(const FString& FullFilePath);

	/**
	* Create game save.
	*
	* This fonction verify if a save exist and create the save.
	* It save the numbers of photos.
	*
	*/
	UFUNCTION()
	void loadGame();

	/**
	* Add photo to a page.
	*
	* This fonction allow us to add the texture of
	* a picture and apply it on the next position available.
	*
	* @param MouseInput verify if the mouse was pressed.
	* @param InputKey value of the number of time the input key was pressed.
	* @param PictureNumber value of the picture number.
	* @param PageSide value that contains which side is active.
	*/
	UFUNCTION(BlueprintCallable, Category="init")
	void AddPhotoToPage(bool MouseInput, int32 InputKey, int32 PictureNumber, UWrapBox* PageSide);

	/**
	* Choose on which page the picture goes.
	*
	* This fonction verify where is the which page is available
	* to add a picture.
	*/
	UFUNCTION(BlueprintCallable, Category="init")
	void CreatePhotoWidget();

	/**
	* Put the focus on the right picture.
	*
	* This fonction verify the position of a picture
	* and put the focus on it.
	*
	* @param MouseInput verify if the mouse was pressed.
	* @param Index Value that contains the position of the picture.
	* @param PageSide value that contains which side is active.
	*/
	UFUNCTION(BlueprintCallable, Category="init")
	void FocusPhoto();

	/**
	* Put the focus on the first picture.
	*
	* This fonction call another fonction to put the
	* fonction on the first picture of the page.
	*
	* @param MouseInput verify if the mouse was pressed.
	*/
	UFUNCTION(BlueprintCallable, Category="init")
	void FocusFirstPhoto();

	UFUNCTION(BlueprintCallable, Category="init")
	void FocusNextPhoto();
	
	UFUNCTION(BlueprintCallable, Category="init")
	void FocusPrevPhoto();

	UFUNCTION(BlueprintCallable, Category="init")
	void FocusTopPhoto();

	UFUNCTION(BlueprintCallable, Category="init")
	void FocusBottomPhoto();
	
	UFUNCTION(BlueprintCallable, Category="Gallery")
	void ChangeButtonText();


	/**
	* Transform the page to fit the images.
	*
	* This fonction is use to transform the current page
	* to fit the size of the picture.
	*
	* @param Index Value that contains the position of the picture.
	*/
	UFUNCTION(BlueprintCallable, Category="Transform")
	void SetWidgetTransform(int32 Index);

	UFUNCTION(BlueprintCallable, Category="Status")
	void SetBoardSelectionMode();
	
	UFUNCTION(BlueprintCallable, Category="init")
	bool SetNextPage();

	UFUNCTION(BlueprintCallable, Category="init")
	bool SetPreviousPage();


	UFUNCTION(Category="init")
	void ToggleIndicePage();

	UFUNCTION(BlueprintCallable, Category="init")
	bool DelFocusPhotoValidation();
	
	UFUNCTION(BlueprintCallable, Category="init")
	bool DelFocusPhoto();
	
	UFUNCTION()
	void displayValidation();

	UFUNCTION()
	void HideValidation();

	UFUNCTION()
	bool changeDisplay();

	UFUNCTION()
	bool NextPhotoExist(int32 index);
	
	UFUNCTION()
	bool NextPageExist();

	UFUNCTION()
	void PlayTurnPageSound();

	UFUNCTION()
	void PlayFocusSound();

	UFUNCTION()
	void PlayZoomSound();

public:
	
	UFUNCTION(BlueprintCallable, Category="Getter")
	int32 GetSelectedPictureNumber();
	
	UFUNCTION(BlueprintCallable, Category="Getter")
	FText GetSelectedPictureDialogue();
	

	//Create a subclass of InstantPhoto
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UCPP_UIInstantPhoto> InstantPhotoClass;

	// Class for the left indice UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> IndiceLeftClass;

	// Class for the rifht indice UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> IndiceRightClass;
	
	//Create a material for the picture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UMaterialInstanceDynamic* PhotoMaterial;

	//Position of the picture
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	TArray<int32> PictureNumbers;

	UPROPERTY(EditAnywhere, Category = "GallerySize")
	TArray<FText> DialogueTextID;

	// Next page text indication on the bottom of the Gallery
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NextPageTxt;

	// previous page text indication on the bottom of the Gallery
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* PrevPageTxt;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Widget")
	class USoundWave* TurnPageSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USoundWave* FocusSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USoundWave* ZoomSound;

private:

	//Used to contains the informations of the left page
	UPROPERTY(EditAnywhere, Category = "GameSave")
	UWrapBox* LeftPage;
	
	//Used to contains the informations of the right page
	UPROPERTY(EditAnywhere, Category = "GameSave")
	UWrapBox* RightPage;

	//Used to contains the informations of the right page
	UPROPERTY(EditAnywhere, Category = "validation")
	UOverlay* DeleteValidationBox;
	
	UPROPERTY(EditAnywhere, Category = "Widget")
	UWidgetSwitcher* Switcher;
	
	UPROPERTY(EditAnywhere, Category = "GameInstance")
	UPhotoGalleryGameInstance* PhotoGameInstance;
	
	// Instance of the ui for the picture
	UPROPERTY(EditAnywhere, Category = "GameSave")
	UCPP_UIInstantPhoto* PhotoWidget;

	// Value for the maximum value of pages
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	int32 MaxPages;
	
	// List containing the pictures
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	TArray<UCPP_UIInstantPhoto*> ListePhotos;

	// Value containing the index of the current page
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	int32 CurrentPage;

	// The pageSide to add a photo to
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	UWrapBox* CurrentPageSide;
	
	// this is no longer used
	UPROPERTY(EditAnywhere, Category = "key")
	int32 CurrentInputKey;

	//the biggest Index in the album
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	int32 photoMaxIndex;

	//the smallest Index in the album
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	int32 photoMinIndex;

	// Focused photo
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	int32 currentPhotoIndex;
	
	// Previous focused photo
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	int32 PreviousPhotoIndex;

	// Current photo
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	UCPP_UIInstantPhoto* CurrentPhoto;

	// Are we using the investigation board
	UPROPERTY(EditAnywhere, Category = "GalleryMode")
	bool BoardSelectionMode;

	// are we validating a choice
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	bool InValidation;

	// are we deleting a photo
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	bool confirmDeletion = false;

	// are we in indice page
	UPROPERTY(EditAnywhere, Category = "GalleryIndiceState")
	bool IsInIndicePage;

	// only play once 	
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	bool NextButtonPressed = true;
	
	// only play once 
	UPROPERTY(EditAnywhere, Category = "GallerySize")
	bool DontPlayOnce = true;
	
public:
	bool IsConfirmDeletion() const;
	void SetConfirmDeletion(bool bConfirmDeletion);
	bool IsInValidation() const;
	void SetInValidation(bool bIsInValidation);
	bool GetIsInIndicePage() const { return IsInIndicePage; }
};
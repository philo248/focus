// Copyright BlindGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UIUserWidget.h"
#include "CPP_UIInstantPhotoFullscreen.generated.h"

class UHorizontalBox;

/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API UCPP_UIInstantPhotoFullscreen : public UUserWidget
{
	GENERATED_BODY()

	public:
	virtual bool Initialize();
	//Position of the picture in the list
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Picture")
	int32 PictureNumber;
	
	//Position of the picture int the gallery
	UPROPERTY(EditAnywhere, Category = "Picture")
	int32 Index = NULL;

	//Verify if the mouse was pressed
	UPROPERTY(EditAnywhere, Category = "Picture")
	bool MouseInput = false;

	//Widget of the frame for the picture
	UPROPERTY(EditAnywhere, Category = "Picture")
	UImage * PhotoFrame11;

	// Dialogue DataTable Reference, set in blueprint, contain all dialogue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* PhrasesTable;

	//Widget that contains the dialogue
	UUserWidget * Dialogue;

	//Block that contains the text
	UTextBlock * DialogueText;

	//Contains the ID for each dialogue
	FName DialogueID;

	//Box to close the text
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UHorizontalBox* CloseTxt;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UHorizontalBox* SelectTxt;
	
	//Widget for the picture
	UPROPERTY(EditAnywhere,Category = "Picture")
	UImage * ImagePhoto;
	bool MouseInput1() const
	{
		return MouseInput;
	}

	int32 InputKey1() const
	{
		return InputKey;
	}

	//Fonction to load the texture
	UFUNCTION(BlueprintCallable, Category="Transform")
	UTexture2D* LoadTextureFromPath1(const FString& Path);

	//Fonction to set the dialogue
	UFUNCTION(BlueprintCallable, Category="Transform")
	void SetDialogueText(FText Dialogue);
	
	//Set the texture for the picture
	void setPicture(int32 PictureNumber);
	//Show the frame of the picture and load the texture on it
	void ShowPicture();

	// Change the text indication of the button on the UI between close photo and select photo
	UFUNCTION()
	void ChangeButtonIndication(bool IsInBoard);

	//TODO::Add those fonction
	void CloseView();
	void NextPicture();
	void PreviousPicture();
	void DeletePictures();
	void GetDialogueText();
	
	//Get the texture of the picture
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Import Image", CompactNodeTitle = "IMG Import", Keywords = "import image texture"), Category = "Instant Camera")
	static UTexture2D *GetTexture(const FString& FullFilePath);

	UPROPERTY(EditAnywhere, Category = "Picture")
	int32 InputKey = 0;
	private:

	
	
};




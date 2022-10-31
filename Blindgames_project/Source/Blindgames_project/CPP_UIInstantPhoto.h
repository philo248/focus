// Copyright BlindGames. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "IImageWrapper.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

#include "CPP_UIInstantPhoto.generated.h"

/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API UCPP_UIInstantPhoto : public UUserWidget
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

	//Variable that contains de Text for the dialogue
	UPROPERTY(EditAnywhere, Category = "Picture")
	FText Dialogue = FText::FromName("test");

	//Widget of the frame for the picture
	UPROPERTY(EditAnywhere, Category = "Picture")
	UImage * PhotoFrame;

	//Picture container
	UPROPERTY(EditAnywhere, Category = "Picture")
	UImage * ImagePhoto;

	//Capture the input
	bool MouseInput1() const
	{
		return MouseInput;
	}

	int32 InputKey1() const
	{
		return InputKey;
	}

	//Get the texture from path
	UFUNCTION(BlueprintCallable, Category="Transform")
	UTexture2D* LoadTextureFromPath1(const FString& Path);

	//Fonction to give focus on a picture
	void Focus();

	//Fonction to unfocus the picture
	void Unfocus();


	UPROPERTY(EditAnywhere, Category = "Picture")
	int32 InputKey = 0;
	private:

	
	
};

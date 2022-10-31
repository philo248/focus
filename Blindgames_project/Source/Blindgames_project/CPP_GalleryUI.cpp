// Copyright BlindGames. All Rights Reserved.

#include "CPP_GalleryUI.h"
#include "CPP_UIInstantPhoto.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "PhotoGalleryGameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "PhotoGallery.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


bool UCPP_GalleryUI::Initialize()
{
    //Parent Class Initialization
	Super::Initialize();
	
	//Removes all photos from list
	for(UCPP_UIInstantPhoto* Photo : ListePhotos)
	{
		Photo->Destruct();
	}
	SetInValidation(false);
	SetConfirmDeletion(false);
	//Empties The UWrapBox containing pointers to every currently displayed photos
	CurrentPage = 1;
	LeftPage =  Cast<UWrapBox>(this->GetWidgetFromName("PhotoBox_Left"));
	LeftPage->ClearChildren();
	RightPage =  Cast<UWrapBox>(this->GetWidgetFromName("PhotoBox_Right"));
	RightPage->ClearChildren();
	Switcher = Cast<UWidgetSwitcher>(this->GetWidgetFromName("TurnPageSwitch"));
	DeleteValidationBox = Cast<UOverlay>(this->GetWidgetFromName("DeleteValidationOverlay"));
	PreviousPhotoIndex = -1;
	currentPhotoIndex = 0;
	BoardSelectionMode = false;
	IsInIndicePage = false;
	DontPlayOnce = true;
	//Loads save Game from memory
	loadGame();
	CreatePhotoWidget();
	Switcher->SetActiveWidgetIndex(1);
	FocusFirstPhoto();

	PrevPageTxt->SetText(FText::FromString("Page indices"));
	
    if (MaxPages > 1)
    {
    	NextPageTxt->SetText(FText::FromString("Page suivante"));
    }
	else
	{
		NextPageTxt->SetText(FText::FromString(" "));
	}
	
	return true;
}

void UCPP_GalleryUI::loadGame()
{
	//Finds and stocks the current GameInstance containing the gamesave
	PhotoGameInstance = Cast<UPhotoGalleryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	//Checks if there is a GameInstance
	if(PhotoGameInstance  != nullptr)
	{
		//Verifies if there is no game save
		if(!PhotoGameInstance->IsNewGame())
		{
			//if true create a new save
			if(!PhotoGameInstance->CreateNewSaveGame())
			{
				UE_LOG(LogTemp, Warning, TEXT("Could not create gamesave"));
			}		
		}
		else
		{
			PhotoGameInstance->LoadGame();
			//Updates the current amount of pages in the gallery
			MaxPages = UKismetMathLibrary::FCeil(PhotoGameInstance->CurrentSaveGame->PictureNumbers.Num()/ 8.f);
			//Get the number of each picture taken from the gamesave
			PictureNumbers = PhotoGameInstance->CurrentSaveGame->PictureNumbers;
			DialogueTextID = PhotoGameInstance->CurrentSaveGame->DialogueTextID;
		}
	}else{
		//if there is no game instance there is only 1 page in the gallery
		MaxPages = 1;
	}
}

void UCPP_GalleryUI::AddPhotoToPage(bool MouseInput, int32 InputKey, int32 PictureNumber, UWrapBox* PageSide)
{
	// Creates the widget for the photo
	PhotoWidget = CreateWidget<UCPP_UIInstantPhoto>(PageSide, InstantPhotoClass);
	// assigns it the proper picture number
	PhotoWidget->PictureNumber = PictureNumber;
	// Adds it to the Page
	PhotoWidget->Dialogue = DialogueTextID[PictureNumbers.Find(PictureNumber)];
	PageSide->AddChild(PhotoWidget);
	//Changes the photoWidgetIndex to be its picture number
	PhotoWidget->Index = PictureNumber;

	//Gets the FilePath to the save Picture and its name
	const FString FilePath = FPaths::ProjectSavedDir() + "Pictures";
	FString FileName =  "/Pic_" + FString::FromInt( PhotoWidget->PictureNumber ) + ".png";

	//Loads the texture, create a M_GalleryPhoto_Master and assign it to the Photo
	UTexture2D* TexturePhoto = GetTexture(FilePath + FileName);
	UMaterial* M_GalleryPhoto_Master = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("Material'/InstantPhoto/Materials/UserInterface/M_GalleryPhoto_Master.M_GalleryPhoto_Master'")));
    UMaterialInstanceDynamic::Create(M_GalleryPhoto_Master, this);
    if (M_GalleryPhoto_Master != nullptr)
    {
        UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(M_GalleryPhoto_Master, this);
        DynamicMat->SetVectorParameterValue("TestColour", FLinearColor(1,0,0));
        DynamicMat->SetTextureParameterValue("Picture", TexturePhoto);
        PhotoWidget->ImagePhoto->SetBrush( UWidgetBlueprintLibrary::MakeBrushFromMaterial(DynamicMat,170,170));
    }
	//Adds the photo to the list of Photos
	ListePhotos.Add(PhotoWidget);
}
//This method was included in the paid downloadable asset plugins
//Im adding the appropriate copyrights
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
UTexture2D* UCPP_GalleryUI::GetTexture(const FString & FullFilePath)
{
	//Gets RAW TEXTURE DATA from stored file
	UTexture2D* LoadedImage = NULL;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath))
	{
		return NULL;
	}

	// creates texture2D from loaded file
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray<uint8> RawData;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
		{
			LoadedImage = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			// checks if loaded file is valid image
			if (!LoadedImage)
			{
				return NULL;
			}

			// copies image
			void* TextureData = LoadedImage->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
			LoadedImage->PlatformData->Mips[0].BulkData.Unlock();

			// updates
			LoadedImage->UpdateResource();
		}
	}
	// returns imported image as texture
	return LoadedImage;
}

void UCPP_GalleryUI::CreatePhotoWidget()
{
	//Searches through every photos in the range
	photoMaxIndex = CurrentPage * 8;
	photoMinIndex = CurrentPage * 8 - 7;
	int32 cpt = 1;
	
	for(int32 PictureNumber : PictureNumbers){
		if(cpt >= photoMinIndex && cpt <= photoMaxIndex)
		{		
			if((cpt -1 )%8 < 4)
			{
				//adds it to leftPage widget
				AddPhotoToPage(true, cpt, PictureNumber, LeftPage );
			}else
			{
				//adds it to RightPage widget
				AddPhotoToPage(true, cpt, PictureNumber, RightPage );
			}				
		}
		cpt++;
	}
}

void UCPP_GalleryUI::FocusPhoto()
{
	
	UWrapBox* CurrentPreviousPage;
	//determines the WrapBox where the photo is
	if(currentPhotoIndex < 4)
	{
		CurrentPageSide=LeftPage;
	}
	else
	{
		CurrentPageSide=RightPage;
	}

	if(PreviousPhotoIndex < 4)
	{
		CurrentPreviousPage = LeftPage;
	}
	else
	{
		CurrentPreviousPage = RightPage;
	}
		//put focus on photo if it exist and is not the last photo selected
		if(CurrentPageSide->GetChildAt(currentPhotoIndex%4)!=nullptr && currentPhotoIndex != PreviousPhotoIndex)
		{
			if (DontPlayOnce) {
				DontPlayOnce = false;
			}
			else {
				PlayFocusSound();
			}
			CurrentPhoto = Cast<UCPP_UIInstantPhoto>(CurrentPageSide->GetChildAt(currentPhotoIndex%4));
			CurrentPhoto->Focus();
			if(PreviousPhotoIndex != -1)
				if(PreviousPhotoIndex >= 0 && PreviousPhotoIndex < 8)
					Cast<UCPP_UIInstantPhoto>(CurrentPreviousPage->GetChildAt(PreviousPhotoIndex%4))->Unfocus();
		}
}

void UCPP_GalleryUI::FocusFirstPhoto()
{
	CurrentPageSide=LeftPage;
	PreviousPhotoIndex = -1;
	currentPhotoIndex=0;
	// puts the focus on a Photo at the first photo 
	FocusPhoto();
}

void UCPP_GalleryUI::FocusNextPhoto()
{
	// is there space for a photo to the right?
	if(currentPhotoIndex%2 == 1)
	{
		// is this the top right photo if so is there a photo on the next page on the same line?
		// if so load next page
		if(currentPhotoIndex == 5 && this->NextPhotoExist(3))
		{	
			PreviousPhotoIndex = -1;
			NextButtonPressed = false;
			currentPhotoIndex-=5;
			SetNextPage();
			FocusPhoto();
		}
		// is this the bottom right photo if so is there another page to load?
		// if so load next page
		else if(currentPhotoIndex == 7 && this->NextPageExist())
		{
			NextButtonPressed = false;
			//is there a photo on the next page on the same line?
			if(this->NextPhotoExist(3))
			{
				PreviousPhotoIndex = -1;
				currentPhotoIndex-=5;
				SetNextPage();
				FocusPhoto();
			}else
			{
				SetNextPage();
				FocusFirstPhoto();
			}				
		}
		// is there a photo to the right?
		else if ( this->NextPhotoExist(3))
		{
			PreviousPhotoIndex = currentPhotoIndex;
			currentPhotoIndex+=3;
			FocusPhoto();
		}

	}
	else if ( this->NextPhotoExist( 1))
	{

		PreviousPhotoIndex = currentPhotoIndex;
		currentPhotoIndex++;
		FocusPhoto();
	}
			
}

void UCPP_GalleryUI::FocusPrevPhoto()
{
	//the indice page is the last page so you cant go any further to the right
	if(!GetIsInIndicePage())
	{
		// are you at the edge of this wrapbox
		if(currentPhotoIndex%2 == 0)
		{
			// are you at the edge of this page
			if(currentPhotoIndex == 0 || currentPhotoIndex == 2)
			{
				PreviousPhotoIndex = -1;
				NextButtonPressed = false;
				currentPhotoIndex+=5;
				PlayTurnPageSound();
				SetPreviousPage();
				FocusPhoto();		
			}
			//check if theres a pic to the left (most likely)
			else if ( this->NextPhotoExist(-3))
			{
				PreviousPhotoIndex = currentPhotoIndex;
				currentPhotoIndex-=3;
				FocusPhoto();
			}

		}
		//check if theres a pic at the current index - 1
		else  if ( this->NextPhotoExist(-1))
		{
			PreviousPhotoIndex = currentPhotoIndex;
			currentPhotoIndex--;
			FocusPhoto();
		}
	}
}

void  UCPP_GalleryUI::FocusTopPhoto()
{
	//check if you are on the bottom line and there is a photo above you
	if((currentPhotoIndex%4==2 ||currentPhotoIndex%4==3)  && this->NextPhotoExist(- 2))
	{
		PreviousPhotoIndex = currentPhotoIndex;
		currentPhotoIndex-=2;
		FocusPhoto();
	}
}

void UCPP_GalleryUI::FocusBottomPhoto()
{
	//check if you are on the top row of the album and there is a photo under you
	if((currentPhotoIndex%4==0 ||currentPhotoIndex%4==1) && this->NextPhotoExist(2))
	{
		PreviousPhotoIndex = currentPhotoIndex;
		currentPhotoIndex+=2;
		FocusPhoto();
	}
}

void UCPP_GalleryUI::ChangeButtonText()
{
	if (CurrentPage >= 1 && CurrentPage < MaxPages) // If the next page is not the last page in the galley
	{
		if (CurrentPage == 1) // If this is the first page and not the last page
		{
			PrevPageTxt->SetText(FText::FromString("Page d'indices"));
			NextPageTxt->SetText(FText::FromString("Page suivante"));
			return;
		}
		PrevPageTxt->SetText(FText::FromString("Page precedente"));
		NextPageTxt->SetText(FText::FromString("Page suivante"));
		return;
	}
	if (CurrentPage == MaxPages || MaxPages==0) // If the next page is the last page in the galley
	{
		if (CurrentPage == 1) // If this is the first page and not the last page
		{
			PrevPageTxt->SetText(FText::FromString("Page d'indices"));
			NextPageTxt->SetText(FText::FromString(""));
			return;
		}

		PrevPageTxt->SetText(FText::FromString("Page precedente"));
		NextPageTxt->SetText(FText::FromString(" "));
		return;
	}

	if (CurrentPage == 1) // If this is the first page and not the last page
	{ 
		PrevPageTxt->SetText(FText::FromString("Page d'indices"));
		NextPageTxt->SetText(FText::FromString("Page suivante"));
		return;
	}
}

bool UCPP_GalleryUI::SetNextPage()
{
	//are we in Indice Page or are there still pages to the album
	if(CurrentPage<MaxPages || IsInIndicePage)
	{
		for(UCPP_UIInstantPhoto* Photo : ListePhotos)
		{
			Photo->Destruct();
		}
		if (IsInIndicePage) // if the current page is the indice, flip page to the first photo page
		{
			CurrentPage = 1;
			IsInIndicePage = false;
			NextButtonPressed = true;
		}
		else
		{
			CurrentPage+=1;
		}
		
		ChangeButtonText();
		LeftPage->ClearChildren();
		RightPage->ClearChildren();
		CreatePhotoWidget();
	


		if(!NextButtonPressed) //if you used the arrow instead of the other input 
		{
			NextButtonPressed = true;
		}
		else
		{
			FocusFirstPhoto();
		}
		PlayTurnPageSound();
		return true;
	}
		return false;

}

bool UCPP_GalleryUI::SetPreviousPage()
{
	
	if(CurrentPage>1) // if current page is not the first photo page
	{
		for(UCPP_UIInstantPhoto* Photo : ListePhotos)
		{
			Photo->Destruct();
		}
		
		CurrentPage-=1;
		LeftPage->ClearChildren();
		RightPage->ClearChildren();
		CreatePhotoWidget();
		ChangeButtonText();
		if(!NextButtonPressed)
		{

			NextButtonPressed = true;
		}
		else
		{	
			PlayTurnPageSound();
			FocusFirstPhoto();
		}

		return true;
	}
	if (NextButtonPressed)
	{
		PlayTurnPageSound();
	}
	// open indices pages
	return changeDisplay();
}



bool UCPP_GalleryUI::changeDisplay()
{
	
	if (!IsInIndicePage) // check if the indices page is already open
	{
		ToggleIndicePage();
		PrevPageTxt->SetText(FText::FromString(" "));
		NextPageTxt->SetText(FText::FromString("Page suivante"));
		return true;
	}
	
	return false;
}

void UCPP_GalleryUI::ToggleIndicePage()
{
	UUserWidget* IndiceLeftWidgetRef = CreateWidget<UUserWidget>(LeftPage, IndiceLeftClass);
	UUserWidget* IndiceRightWidgetRef = CreateWidget<UUserWidget>(RightPage, IndiceRightClass);

	if (IndiceLeftClass && IndiceRightClass) // validate that both Widgets are properly instantiated
	{
		LeftPage->ClearChildren();
		RightPage->ClearChildren();
		LeftPage->AddChild(IndiceLeftWidgetRef);
		RightPage->AddChild(IndiceRightWidgetRef);
		CurrentPage = 0;
		IsInIndicePage = true;
	}
}

bool UCPP_GalleryUI::DelFocusPhotoValidation()
{
	bool DeleteSuccess = false;

	if (CurrentPhoto == nullptr || IsInIndicePage)
	{
		return DeleteSuccess;
	}
	
	if(!IsInValidation())
	{
		displayValidation();
	}
	else
	{
		if(IsConfirmDeletion())
		{
			DeleteSuccess = DelFocusPhoto();
			Initialize();
		}
		HideValidation();
	}
	return DeleteSuccess;
}

bool UCPP_GalleryUI::DelFocusPhoto()
{
	int32 index = PhotoGameInstance->CurrentSaveGame->PictureNumbers.Find(CurrentPhoto->PictureNumber);
	PhotoGameInstance->CurrentSaveGame->DialogueTextID.RemoveAt(index);
	PhotoGameInstance->CurrentSaveGame->PictureNumbers.Remove(CurrentPhoto->PictureNumber);
	PhotoGameInstance->SaveGame();
	CurrentPhoto = nullptr;

	return true;
}

void UCPP_GalleryUI::SetWidgetTransform(int32 Index)
{
	//Gives different transforms and Angle to a photo depending on its position in the Gallery

	//determines the position of the page
	int32 A = Index - (CurrentPage - 1 * 8);
	int32 B = abs(A - 7);
	FWidgetTransform WTransform;
	FVector2D FrameScale;
	if(!(CurrentPage%2 > 0))
	{
		A = B;
	}
	//Gets the correct transforms and Angles for the particular position.
	switch(A)
	{
		case 0:
			WTransform = FWidgetTransform(FVector2D(4.0f, 8.0f), FVector2D(1.05f, 1.05f),FVector2D(0.0f, 0.0f), -6);
			break;
		case 1:
			WTransform = FWidgetTransform(FVector2D(-2.0f, -4.0f), FVector2D(1.05f, 1.05f),FVector2D(0.0f, 0.0f), -6);
			break;
		case 2:
			WTransform = FWidgetTransform(FVector2D(-14.0f, 2.0f), FVector2D(1.05f, 1.05f),FVector2D(0.0f, 0.0f), -6);
			break;
		case 3:
			WTransform = FWidgetTransform(FVector2D(-5.0f, 8.0f), FVector2D(1.05f, 1.05f),FVector2D(0.0f, 0.0f), -6);
			break;
		case 4:
			WTransform = FWidgetTransform(FVector2D(4.0f, 8.0f), FVector2D(1.05f, 1.05f),FVector2D(0.0f, 0.0f), -6);
			break;
		case 5:
			WTransform = FWidgetTransform(FVector2D(2.0f, -13.0f), FVector2D(1.05f, 1.05f),FVector2D(0.0f, 0.0f), -6);
			break;
		case 6:
			WTransform = FWidgetTransform(FVector2D(-5.0f, 2.0f), FVector2D(1.05f, 1.05f),FVector2D(0.0f, 0.0f), -6);
			break;
		case 7:
			WTransform = FWidgetTransform(FVector2D(2.0f, -5.0f), FVector2D(1.05f, 1.05f),FVector2D(0.0f, 0.0f), -6);
			break;
		default:
			break;
	}
	//Assigns it to the photo
	PhotoWidget->SetRenderTransform(WTransform);

	//Changes the scale of the 3rd photo frame of both sides of the gallery
	if(LeftPage->GetChildrenCount() == 3 || RightPage->GetChildrenCount() == 3)
	{
		FrameScale = FVector2D(1.0,1.0);
	}else
	{
		FrameScale = FVector2D(-1.0,1.0);
	}
	
	PhotoWidget->PhotoFrame->SetRenderScale(FrameScale);
}

void UCPP_GalleryUI::displayValidation()
{
	SetInValidation(true);
	SetConfirmDeletion(false);
	DeleteValidationBox->SetVisibility(ESlateVisibility::Visible);
}

void UCPP_GalleryUI::HideValidation()
{
	SetInValidation(false);
	DeleteValidationBox->SetVisibility(ESlateVisibility::Hidden);
}


int32 UCPP_GalleryUI::GetSelectedPictureNumber()
{
	int32 picnum;
	if (CurrentPhoto != nullptr)
	{
		picnum = CurrentPhoto->PictureNumber;
	}
	else
	{
		picnum = 0;
	}
	
	return picnum;
}

FText UCPP_GalleryUI::GetSelectedPictureDialogue()
{
	FText dialogue;
	if (CurrentPhoto != nullptr)
	{
		dialogue = CurrentPhoto->Dialogue;
	}
	else
	{
		dialogue = FText();
	}
	
	return dialogue;
}

bool UCPP_GalleryUI::IsConfirmDeletion() const
{
	return confirmDeletion;
}

void UCPP_GalleryUI::SetConfirmDeletion(bool bConfirmDeletion)
{
	confirmDeletion = bConfirmDeletion;
}

bool UCPP_GalleryUI::IsInValidation() const
{
	return InValidation;
}

void UCPP_GalleryUI::SetInValidation(bool bInValidation)
{
	InValidation = bInValidation;
}


void UCPP_GalleryUI::SetBoardSelectionMode()
{
	BoardSelectionMode = true;
}

bool UCPP_GalleryUI::NextPhotoExist(int32 offset)
{
	return PictureNumbers.IsValidIndex(currentPhotoIndex+ 8 * (CurrentPage - 1) + offset);
}

bool UCPP_GalleryUI::NextPageExist()
{
	return PictureNumbers.IsValidIndex(8 * CurrentPage);
}

void UCPP_GalleryUI::PlayTurnPageSound()
{
	UGameplayStatics::PlaySound2D(this, TurnPageSound);
}

void UCPP_GalleryUI::PlayFocusSound() 
{
	UGameplayStatics::PlaySound2D(this, FocusSound);
}

void UCPP_GalleryUI::PlayZoomSound()
{
	UGameplayStatics::PlaySound2D(this, ZoomSound);
}






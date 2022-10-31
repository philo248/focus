// Copyright BlindGames. All Rights Reserved.


#include "CPP_UIInstantPhotoFullscreen.h"

#include "CustomGameMode.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "QuestData.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

bool UCPP_UIInstantPhotoFullscreen::Initialize()
{
	//Parent Class Initialization
	Super::Initialize();
	
	PhotoFrame11 =  Cast<UImage>(this->GetWidgetFromName("PhotoFrame"));
	ImagePhoto =  Cast<UImage>(this->GetWidgetFromName("Picture"));
	Dialogue =  Cast<UUserWidget>(this->GetWidgetFromName("UI_PhotoFullscreenDialogue"));
	DialogueText = Cast<UTextBlock>(Dialogue->GetWidgetFromName("DialogueText"));
	
	return true;
}

//This method was included in the paid downloadable asset plugins
//Im adding the appropriate copyrights
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
UTexture2D * UCPP_UIInstantPhotoFullscreen::GetTexture(const FString & FullFilePath)
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

UTexture2D* UCPP_UIInstantPhotoFullscreen::LoadTextureFromPath1(const FString& Path)
{
	if (Path.IsEmpty()) return NULL;
 
	return Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(Path)));
}

void UCPP_UIInstantPhotoFullscreen::setPicture(int32 PicNumber)
{
	//TODO:: assigns it the proper picture number

	PictureNumber = PicNumber;

	//Gets the FilePath to the save Picture and its name
	const FString FilePath = FPaths::ProjectSavedDir() + "Pictures";
	FString FileName =  "/Pic_" + FString::FromInt( PicNumber ) + ".png";

	//Loads the texture, create a M_GalleryPhoto_Master and assign it to the Photo
	UTexture2D * TexturePhoto = GetTexture(FilePath + FileName);
	UMaterial* M_GalleryPhoto_Master = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("Material'/InstantPhoto/Materials/UserInterface/M_GalleryPhoto_Master.M_GalleryPhoto_Master'")));
	UMaterialInstanceDynamic::Create(M_GalleryPhoto_Master, this);
	if (M_GalleryPhoto_Master != nullptr)
	{
		UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(M_GalleryPhoto_Master, this);
		DynamicMat->SetVectorParameterValue("TestColour", FLinearColor(1,0,0));
		DynamicMat->SetTextureParameterValue("Picture", TexturePhoto);
		ImagePhoto->SetBrush( UWidgetBlueprintLibrary::MakeBrushFromMaterial(DynamicMat,170,170));
	}
	
}

//Set the picture on the frame
void UCPP_UIInstantPhotoFullscreen::ShowPicture()
{
	FString  PathToLoad = "/Game/Widgets/UserInterface/Album/T_Photo_01.T_Photo_01";
	UTexture2D * TextureHover = LoadTextureFromPath1(PathToLoad);
	PhotoFrame11->SetBrushFromTexture(TextureHover);
}

void UCPP_UIInstantPhotoFullscreen::SetDialogueText(FText DialoguePhoto)
{
	DialogueText->SetText(DialoguePhoto);
}

void UCPP_UIInstantPhotoFullscreen::GetDialogueText()
{
	ACustomGameMode* GameMode = Cast<ACustomGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode  != nullptr) // Just to make sure game mode pointer is valid
	{
		for(int i = 0; i < GameMode->Quest->QuestDataList.Num(); i++)
		{
			for(int32 pic : GameMode->Quest->QuestDataList[i].PicturesNumbRef)
			{
				if(PictureNumber == pic)
				{
					DialogueID = GameMode->Quest->QuestDataList[i].DistortionActorTag;
					break;
				}
			}
		}
	}
}

//Change the indication for the button
void UCPP_UIInstantPhotoFullscreen::ChangeButtonIndication(bool IsInBoard)
{
	if (IsInBoard)
	{
		CloseTxt->SetVisibility(ESlateVisibility::Hidden);
		SelectTxt->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CloseTxt->SetVisibility(ESlateVisibility::Visible);
		SelectTxt->SetVisibility(ESlateVisibility::Hidden);
	}
}



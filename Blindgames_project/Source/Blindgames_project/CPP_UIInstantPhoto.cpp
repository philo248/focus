// Copyright BlindGames. All Rights Reserved.

/*
* This Class contains the methods and variables
* Used within The Gallery UI.
*/


#include "CPP_UIInstantPhoto.h"


bool UCPP_UIInstantPhoto::Initialize()
{
	//Parent Class Initialization
	Super::Initialize();
	PhotoFrame =  Cast<UImage>(this->GetWidgetFromName("Photo_Frame"));
	ImagePhoto =  Cast<UImage>(this->GetWidgetFromName("Photo"));
	return true;
	
}

//Fonction that load the texture from the path
UTexture2D* UCPP_UIInstantPhoto::LoadTextureFromPath1(const FString& Path)
{
	
	if (Path.IsEmpty()) return NULL;
	//Cast the texture
	return Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(Path)));
}

void UCPP_UIInstantPhoto::Focus()
{
	FString  PathToLoad = "/Game/Widgets/UserInterface/Album/T_Photo_01_HoverTest.T_Photo_01_HoverTest";
	UTexture2D * TextureHover = LoadTextureFromPath1(PathToLoad);
	PhotoFrame->SetBrushFromTexture(TextureHover);
}

void UCPP_UIInstantPhoto::Unfocus()
{
	FString  PathToLoad = "/Game/Widgets/UserInterface/Album/T_Photo_01.T_Photo_01";
	UTexture2D * TextureHover = LoadTextureFromPath1(PathToLoad);
	PhotoFrame->SetBrushFromTexture(TextureHover);
}

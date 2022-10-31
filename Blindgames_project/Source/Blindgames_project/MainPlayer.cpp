// Copyright BlindGames. All Rights Reserved.

#include "MainPlayer.h"

#include "CPP_GalleryUI.h"
#include "InGameHUD.h"
#include "Components/CapsuleComponent.h"
#include "InstantCameraComponent.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	CameraComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("InstantCameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);

	IsInPhotoMode = false;
	CanOpenBoard = false;
}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	AInstantCameraComponent* CamRef = Cast<AInstantCameraComponent>(CameraComponent->GetChildActor());
	if (CamRef)
	{
		CameraRef = CamRef;
	}

	// Set player HUD reference
	GameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	EnableInput(GetWorld()->GetFirstPlayerController());
	
	Super::BeginPlay();
}

void AMainPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	MainPlayerInputComponent = PlayerInputComponent;
	check(MainPlayerInputComponent);

	// Bind movement events
	MainPlayerInputComponent->BindAxis("MoveForward", this, &AMainPlayer::MoveForward);
	MainPlayerInputComponent->BindAxis("MoveRight", this, &AMainPlayer::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	MainPlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	MainPlayerInputComponent->BindAxis("TurnRate", this, &AMainPlayer::TurnAtRate);
	MainPlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	MainPlayerInputComponent->BindAxis("LookUpRate", this, &AMainPlayer::LookUpAtRate);

	// Camera input events
	//MainPlayerInputComponent->BindAction("SetCamera", IE_Pressed, this, &AMainPlayer::CameraAction);
	MainPlayerInputComponent->BindAction("ShootPhoto", IE_Released, this, &AMainPlayer::TakePicture);
	MainPlayerInputComponent->BindAction("ToggleCameraFlash", IE_Pressed, this, &AMainPlayer::ToggleCameraFlash);
	
	// Gallery input event
	//MainPlayerInputComponent->BindAction("Gallery", IE_Released, this, &AMainPlayer::ToggleGallery);
	MainPlayerInputComponent->BindAction("GalleryNextPage", IE_Released, this, &AMainPlayer::NextPage);
	MainPlayerInputComponent->BindAction("GalleryPrevPage", IE_Released, this, &AMainPlayer::PrevPage);
	MainPlayerInputComponent->BindAction("FocusNextPhoto", IE_Released, this, &AMainPlayer::PrevPhoto);
	MainPlayerInputComponent->BindAction("FocusPrevPhoto", IE_Released, this, &AMainPlayer::NextPhoto);
	MainPlayerInputComponent->BindAction("FocusTopPhoto", IE_Released, this, &AMainPlayer::TopPhoto);
	MainPlayerInputComponent->BindAction("FocusBottomPhoto", IE_Released, this, &AMainPlayer::BottomPhoto);
	MainPlayerInputComponent->BindAction("DelPhoto", IE_Released, this, &AMainPlayer::DelPhoto);
	MainPlayerInputComponent->BindAction("ZoomPhoto", IE_Released, this, &AMainPlayer::ZoomPhoto);
	MainPlayerInputComponent->BindAction("Cancel", IE_Released, this, &AMainPlayer::Cancel);
	
	// Menu pause input events
	MainPlayerInputComponent->BindAction("Pause", IE_Released, this, &AMainPlayer::ToggleMenuPause);

	// Display board
	MainPlayerInputComponent->BindAction("Confirm", IE_Pressed, this, &AMainPlayer::DisplayBoard);
}

void AMainPlayer::EnableCameraControl()
{
	if (MainPlayerInputComponent)
	{
		// Camera input events
		MainPlayerInputComponent->BindAction("SetCamera", IE_Pressed, this, &AMainPlayer::CameraAction);
		// Gallery input event
		MainPlayerInputComponent->BindAction("Gallery", IE_Released, this, &AMainPlayer::ToggleGallery);
	}
}

void AMainPlayer::ToggleGallery()
{
	if (GameHUD)
	{
		if (!IsInPhotoMode && !GameHUD->isPhotoFullscreen)
		{
			GameHUD->ToggleGallery();
		}
	}
}

void AMainPlayer::NextPage()
{
	if(GameHUD->IsInGalleryMode && !GameHUD->isPhotoFullscreen)
	{
		GameHUD->NextPage();
	}
}

void AMainPlayer::PrevPage()
{
	if(GameHUD->IsInGalleryMode && !GameHUD->isPhotoFullscreen)
	{
		GameHUD->PrevPage();
	}
}

void AMainPlayer::NextPhoto()
{
	if(GameHUD->IsInGalleryMode && !GameHUD->isPhotoFullscreen)
	{
		GameHUD->NextPhoto();
	}
}

void AMainPlayer::PrevPhoto()
{
	if(GameHUD->IsInGalleryMode && !GameHUD->isPhotoFullscreen)
	{
		GameHUD->PrevPhoto();
	}
}

void AMainPlayer::TopPhoto()
{
	UCPP_GalleryUI* GalleryUI = Cast<UCPP_GalleryUI>(GameHUD->CurrentWidget);
	if(GameHUD->IsInGalleryMode)
	{
		if(!GameHUD->isPhotoFullscreen && !GalleryUI->IsInValidation())
			{
			GameHUD->TopPhoto();
			}
	}
}

void AMainPlayer::BottomPhoto()
{
	UCPP_GalleryUI* GalleryUI = Cast<UCPP_GalleryUI>(GameHUD->CurrentWidget);
	if(GameHUD->IsInGalleryMode)
	{
		if(!GameHUD->isPhotoFullscreen && !GalleryUI->IsInValidation())
			{
			GameHUD->BottomPhoto();
			}
	}
}

void AMainPlayer::DelPhoto()
{
	UCPP_GalleryUI* GalleryUI = Cast<UCPP_GalleryUI>(GameHUD->CurrentWidget);
	if(GameHUD->IsInGalleryMode)
	{
		if(!GameHUD->isPhotoFullscreen && !GalleryUI->IsInValidation())
		{
			GameHUD->DelPhoto();
		}
	}
}

void AMainPlayer::ZoomPhoto()
{
	UCPP_GalleryUI* GalleryUI = Cast<UCPP_GalleryUI>(GameHUD->CurrentWidget);
	
	if (GameHUD)
	{
		if(GameHUD->IsInGalleryMode)
		{
			if(!GalleryUI->IsInValidation()) // Check if the validation box UI is open or not
			{
				if (!GameHUD->isPhotoFullscreen || GameHUD->isPhotoFullscreen && GameHUD->isSorting)
				{
					GameHUD->ToggleZoomPhoto();
				}
			}
			else
			{
				GalleryUI->SetConfirmDeletion(true);
				GameHUD->DelPhoto();
			}
		}
	}
}

void AMainPlayer::DisplayBoard()
{
	if (CanOpenBoard && !IsInPhotoMode && !GameHUD->isSorting)
	{
		GameHUD->DisplayBoardView();
	}
}

void AMainPlayer::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMainPlayer::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMainPlayer::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainPlayer::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainPlayer::CameraAction()
{
	if (!CanOpenBoard && !GameHUD->IsInGalleryMode)
	{
		CameraRef->CheckIfCameraIsSet();
	}
}

void AMainPlayer::Cancel()
{
	UCPP_GalleryUI* GalleryUI = Cast<UCPP_GalleryUI>(GameHUD->CurrentWidget);
	
	if(GameHUD->isPhotoFullscreen) // If the player is in the gallery and a photo is full screen
	{
		GameHUD->SetKeepGalleryOpen(true);
		GameHUD->ToggleZoomPhoto();
	}
	else if(GameHUD->IsInGalleryMode) // If the player is in the gallery 
	{
		if (!IsInPhotoMode  && !GalleryUI->IsInValidation()) // Check if the validation box UI is open or not
		{
			GameHUD->ToggleGallery();
		}
		else if (GalleryUI->IsInValidation())
		{
			GalleryUI->SetConfirmDeletion(false);
			Cast<UCPP_GalleryUI>(GameHUD->CurrentWidget)->DelFocusPhotoValidation();
		}
	}
}

void AMainPlayer::TakePicture()
{
	if (IsInPhotoMode)
	{
		CameraRef->ShootAndReceivePicture();
	}
}

void AMainPlayer::ToggleMenuPause()
{
	if (GameHUD)
	{
		GameHUD->ShowMenuPause();
	}
}

void AMainPlayer::ToggleCameraFlash()
{
	if (IsInPhotoMode)
	{
		if (CameraRef)
		{
			bool Flashstatus = CameraRef->ToggleFlash();
			
			if (GameHUD)
			{
				GameHUD->ToggleFlashIcon(Flashstatus);	
			}
		}
	}
}

// Copyright BlindGames. All Rights Reserved.

#include "InGameHUD.h"
#include "Board.h"
#include "CPP_GalleryUI.h"
#include "CPP_UIInstantPhotoFullscreen.h"
#include "CustomGameMode.h"
#include "MainPlayer.h"
#include "UIBoard.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UIDialogue.h"
#include "Viewfinder.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AInGameHUD::AInGameHUD() 
{
	PlayerControllerRef = UGameplayStatics::GetPlayerController(this, 0);

	// Widget class references and sounds are set in the blueprint to allow artists to access it
	
	FirstTimeBoard = true;
	isSorting = false;
	OverlayWidget = nullptr;
}

void AInGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AInGameHUD::StartGameHUD()
{
	if (BaseViewClass)
	{
		if (PlayerControllerRef)
		{
			CurrentWidget = CreateWidget<UUserWidget>(PlayerControllerRef, BaseViewClass); // Create base view HUD

			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();
			}
		}
	}
}

void AInGameHUD::EnableCameraView()
{
	if (CameraViewClass)
	{
		if (PlayerControllerRef)
		{
			CurrentWidget->RemoveFromParent();

			CurrentWidget = CreateWidget<UUserWidget>(PlayerControllerRef, CameraViewClass); // Enable camera viewfinder

			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();
			}
		}
	}
}

void AInGameHUD::DisableCameraView()
{
	if (BaseViewClass)
	{
		if (PlayerControllerRef)
		{
			UViewfinder* ViewfinderRef = Cast<UViewfinder>(CurrentWidget);

			if (ViewfinderRef)
			{
				UWidgetAnimation* Anim = ViewfinderRef->FadeAnim;
				ViewfinderRef->PlayAnimation(Anim, 0.7f, 1, EUMGSequencePlayMode::Reverse, 1.f, false); // Fade out the camera view
			}

			FTimerHandle handle; // Timer to wait for the end of the fade out anim, add baseview HUD after
			GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
				CurrentWidget->RemoveFromParent();

				CurrentWidget = CreateWidget<UUserWidget>(PlayerControllerRef, BaseViewClass);

				if (CurrentWidget)
				{
					CurrentWidget->AddToViewport();
				}
				}, 0.4f, false, .8f);
		}
	}
}

void AInGameHUD::ToggleFlashIcon(bool FlashStatus)
{
	UViewfinder* ViewfinderRef = Cast<UViewfinder>(CurrentWidget);

	if (ViewfinderRef)
	{
		ViewfinderRef->ToggleFlashIconVisibility(FlashStatus);
	}
}

void AInGameHUD::ToggleGallery()
{
	// Do this if the gallery is open
	if (CurrentWidget->GetClass() == PhotoGalleryClass)
	{
		if (PlayerControllerRef)
		{
			if (isSorting)
			{
				if (CloseBookSound)
				{
					UGameplayStatics::PlaySound2D(this, CloseBookSound);
				}
				
				DisplayBoardView();
			}
			else
			{
				CurrentWidget->RemoveFromParent();
				
				CurrentWidget = CreateWidget<UUserWidget>(PlayerControllerRef, BaseViewClass);
				
				if (CurrentWidget)
				{
					CurrentWidget->AddToViewport();

					if (CloseBookSound)
					{
						UGameplayStatics::PlaySound2D(this, CloseBookSound);
					}
				}

				// Set input mode to Game
				FInputModeGameOnly InputModeGame;
				PlayerControllerRef->SetInputMode(InputModeGame);
				PlayerControllerRef->SetIgnoreMoveInput(false);
			}
			IsInGalleryMode=false;
		}
	}
	// Do this if the gallery is not open
	else
	{
		if (PlayerControllerRef && CurrentWidget->GetClass() != AccessBoardClass)
		{
			CurrentWidget->RemoveFromParent();

			CurrentWidget = CreateWidget<UUserWidget>(PlayerControllerRef, PhotoGalleryClass);

			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();

				if (OpenBookSound)
				{
					UGameplayStatics::PlaySound2D(this, OpenBookSound);
				}
			}
			
			// Set input mode to UI and set focus to the photo gallery widget
			FInputModeGameAndUI InputModeGameAndUI;
			PlayerControllerRef->SetInputMode(InputModeGameAndUI);
			PlayerControllerRef->SetIgnoreMoveInput(true);
			
			IsInGalleryMode=true;
		}
	}
}

void AInGameHUD::NextPage()
{
	UCPP_GalleryUI* GalleryRef = Cast<UCPP_GalleryUI>(CurrentWidget);

	if (GalleryRef && !GalleryRef->IsInValidation())
	{
		if (TurnPageSound && GalleryRef->SetNextPage())
		{
			
		}
	}
}

void AInGameHUD::PrevPage()
{
	UCPP_GalleryUI* GalleryRef = Cast<UCPP_GalleryUI>(CurrentWidget);

	if (GalleryRef && !GalleryRef->IsInValidation())
	{
		if (TurnPageSound && GalleryRef->SetPreviousPage())
		{
			
		}
	}
}

void AInGameHUD::NextPhoto()
{
	UCPP_GalleryUI* GalleryRef = Cast<UCPP_GalleryUI>(CurrentWidget);

	if (GalleryRef)
	{
		if(!GalleryRef->GetIsInIndicePage())
		{
			GalleryRef->FocusNextPhoto();
		}else
		{
			NextPage();
		}
	}
}

void AInGameHUD::TopPhoto()
{
	UCPP_GalleryUI* GalleryRef = Cast<UCPP_GalleryUI>(CurrentWidget);

	if (GalleryRef)
	{
		if (!GalleryRef->GetIsInIndicePage())
		{
			GalleryRef->FocusTopPhoto();
		}
	}
}

void AInGameHUD::BottomPhoto()
{
	UCPP_GalleryUI* GalleryRef = Cast<UCPP_GalleryUI>(CurrentWidget);

	if (GalleryRef)
	{
		if (!GalleryRef->GetIsInIndicePage())
		{
			GalleryRef->FocusBottomPhoto();
		}
	}
}

void AInGameHUD::PrevPhoto()
{
	UCPP_GalleryUI* GalleryRef = Cast<UCPP_GalleryUI>(CurrentWidget);

	if (GalleryRef)
	{
		if (!GalleryRef->GetIsInIndicePage())
		{
			GalleryRef->FocusPrevPhoto();
		}
	}
}

void AInGameHUD::DelPhoto()
{
	UCPP_GalleryUI* GalleryRef = Cast<UCPP_GalleryUI>(CurrentWidget);

	if (GalleryRef)
	{
		GalleryRef->DelFocusPhotoValidation();
		if (TurnPageSound)
		{
			//TODO::UGameplayStatics::PlaySound2D(this, TurnPageSound);
		}
	}
}

bool AInGameHUD::ToggleZoomPhoto()
{

	if (OverlayWidget != nullptr) // If a photo is zoom in
	{
		if(isSorting && !KeepGalleryOpen) // If the player is sorting photos at the board, call function to check if the selected photo is valid for a quest
		{
			bool QuestValidation = ValidateQuestMessage();
			
			IsInGalleryMode=false;
			DisplayBoardView();
			
			if (QuestValidation) // If a quest is completed, disable Quest button for the quest that is now completed
			{
				if (GoodSound)
				{
					UGameplayStatics::PlaySound2D(this, GoodSound);
				}
				
				UUIBoard* BoardUIRef = Cast<UUIBoard>(CurrentWidget);
				if (BoardUIRef)
				{
					BoardUIRef->DisableButton(QuestButtonID);
					ACustomGameMode* GameMode = Cast<ACustomGameMode>(GetWorld()->GetAuthGameMode());
					GameMode->CompletQuest(QuestButtonID);

					if (GameMode->CheckIfAllQuestAreCompleted())
					{
						BoardUIRef->EnableDistortionButton();
					}
				}
			}
			else
			{
				if (BadSound)
				{
					UGameplayStatics::PlaySound2D(this, BadSound);
				}
			}
		}
		else // If the player is not in the sorting board, normal activity
		{
			
			SetKeepGalleryOpen(false);
		}

		if (PlayerControllerRef) // Remove zoomed photo from the screen
		{
			OverlayWidget->RemoveFromParent();
			OverlayWidget = nullptr;
			isPhotoFullscreen=false;
			UGameplayStatics::PlaySound2D(this, ZoomSound);
			return true;
		}

		return false;
	}
	else // If no photo is zoom in
	{
		if (PlayerControllerRef)
		{
			UCPP_GalleryUI* Gallery = Cast<UCPP_GalleryUI>(CurrentWidget);
			
			if (Gallery)
			{
				if (Gallery->GetIsInIndicePage())
				{
					UGameplayStatics::PlaySound2D(this, ZoomSound);
					return false;
				}
				
				SelectedPicture = Gallery->GetSelectedPictureNumber();
				FText Dialogue = Gallery->GetSelectedPictureDialogue();
				
				if (SelectedPicture == 0)
				{
					UGameplayStatics::PlaySound2D(this, ZoomSound);
					return false;
				}
				
				OverlayWidget = CreateWidget<UUserWidget>(PlayerControllerRef, PhotoFullscreen);
				
				if (OverlayWidget)
				{
					Cast<UCPP_UIInstantPhotoFullscreen>(OverlayWidget)->setPicture(SelectedPicture);
					Cast<UCPP_UIInstantPhotoFullscreen>(OverlayWidget)->GetDialogueText();
					Cast<UCPP_UIInstantPhotoFullscreen>(OverlayWidget)->SetDialogueText(Dialogue);
					Cast<UCPP_UIInstantPhotoFullscreen>(OverlayWidget)->ChangeButtonIndication(isSorting);
					OverlayWidget->AddToViewport();
					isPhotoFullscreen=true;
					UGameplayStatics::PlaySound2D(this, ZoomSound);
					return true;
				}
			}
		}
		UGameplayStatics::PlaySound2D(this, ZoomSound);
		return false;
	}
}

bool AInGameHUD::ValidateQuestMessage()
{
	ACustomGameMode* GameMode = Cast<ACustomGameMode>(GetWorld()->GetAuthGameMode());

	if (!GameMode->CheckIfQuestContainsPhoto(QuestButtonID)) // check if the quest contain valid photo, if not return false and told the player to return take more photo
	{
		const char* ID = "Board_novalidphoto";
		ShowDialogue(ID);
		return false;
	}
	
	bool PhotoIsValid = GameMode->CheckIfPhotoIsValid(SelectedPicture);
	bool PhotoIsInQuest = GameMode->CheckIfPhotoIsInQuest(SelectedPicture, QuestButtonID);
	
	if (PhotoIsValid) // If the selected photo is valid and exist inside a quest
	{
		if (PhotoIsInQuest) // If the selected photo is valid and the selected quest is valid
		{
			TArray<AActor*> FoundBoards;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), BoardClass, FoundBoards); // find the sorting board in world
			
			if (FoundBoards[0] != nullptr) // Make visible the photo on the board and change material to the actual photo
			{
				ABoard* BoardRef = Cast<ABoard>(FoundBoards[0]);
				BoardRef->GetAndChangePhotoMaterial(SelectedPicture, QuestButtonID);
			}

			const char* ID;
			
			switch (QuestButtonID) // Switch to find which quest is completed, Show a different message for each quest
			{
				case 0 :
					ID = "Board_goodsituation"; // Situation Quest completed
					ShowDialogue(ID);
					break;
				case 1:
					ID = "Board_goodemotion"; // Emotion quest completed
					ShowDialogue(ID);
					break;
				case 2:
					ID = "Board_goodpensee"; // Pensée quest completed
					ShowDialogue(ID);
					break;
				default: ;
			}
			
			return true;
		}
		else // If the selected photo is valid, but the selected quest is wrong
		{
			const char* ID = "Board_missplacedphoto";
			ShowDialogue(ID);
			return false;
		}
	}
	else // If the selected photo is completely wrong
	{
		const char* ID = "Board_badphoto";
		ShowDialogue(ID);
		return false;
	}
}

void AInGameHUD::ShowMenuPause()
{
	if (PlayerControllerRef)
	{
		UUserWidget* MenuWidget = CreateWidget<UUserWidget>(PlayerControllerRef, PauseMenuClass);

		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
		}
	}
}

void AInGameHUD::AccessBoardView()
{
	AMainPlayer* CharacterRef = Cast<AMainPlayer>(PlayerControllerRef->GetPawn());
	if (CharacterRef->GetCanOpenBoard())
	{
		if (PlayerControllerRef)
		{
			CurrentWidget->RemoveFromParent();
			CurrentWidget = CreateWidget<UUserWidget>(PlayerControllerRef, AccessBoardClass);

			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();
			}
		}
	}
	else
	{
		EndAccessBoardView();
	}
}

void AInGameHUD::DisplayBoardView()
{
	if (PlayerControllerRef)
	{
		// Set input mode to UI and set focus to the photo gallery widget
		FInputModeUIOnly InputModeUI;
		PlayerControllerRef->SetInputMode(InputModeUI);
		
		CurrentWidget->RemoveFromParent();
		
		if (FirstTimeBoard)
		{
			CurrentWidget = CreateWidget<UUserWidget>(PlayerControllerRef, FirstTimeDisplayBoardClass);

			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();
				FirstTimeBoard = false;
			}
		}
		else
		{
			CurrentWidget = CreateWidget<UUserWidget>(PlayerControllerRef, DisplayBoardClass);

			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();
			}
		}
	}
}

void AInGameHUD::DisplayDistortionCognitiveView()
{
	if (PlayerControllerRef)
	{
		CurrentWidget = CreateWidget<UUserWidget>(PlayerControllerRef, SelectDistortionCognitiveClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void AInGameHUD::EndAccessBoardView()
{
	CurrentWidget->RemoveFromParent();
	StartGameHUD();
}

FText AInGameHUD::GetDialogue()
{
	return DialogueUI->GetDialogue();
}

void AInGameHUD::ShowDialogue(FName LineID)
{
	if (PlayerControllerRef)
	{
		if (LineID == "BBEG_Fig1" || LineID == "BBEG_Fig2" || LineID == "BBEG_Fig3" )
		{
			DialogueUI = Cast<UUIDialogue>(CreateWidget<UUserWidget>(PlayerControllerRef, DialogueBoardClass));
		}
		else
		{
			// Show Board dialogue UI if the player is sorting on the board, or show normal dialogue UI if not
			(isSorting) ? (DialogueUI = Cast<UUIDialogue>(CreateWidget<UUserWidget>(PlayerControllerRef, DialogueBoardClass)))
				: (DialogueUI = Cast<UUIDialogue>(CreateWidget<UUserWidget>(PlayerControllerRef, DialogueClass)));
		}
		
		if (DialogueUI)
		{
			DialogueUI->ChangeTextByID(LineID);
			DialogueUI->AddToViewport();
			
			if(DialogueUI->GetClass() == DialogueBoardClass) // Hide Board dialogue UI if the player is sorting on the board
			{
				FTimerHandle handle;
				GetWorld()->GetTimerManager().SetTimer(handle, [this]()
				{
					TArray<UUserWidget*> Widgets;
					UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, DialogueBoardClass);

					if (Widgets.IsValidIndex(0))
					{
						Widgets[0]->RemoveFromParent();
					}
				}, 6.0f, false);
			}
			else // Hide normal dialogue UI if the player is not sorting on the board
			{
				FTimerHandle handle;
				GetWorld()->GetTimerManager().SetTimer(handle, [this]()
				{
					TArray<UUserWidget*> Widgets;
					UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, DialogueClass);

					if (Widgets.IsValidIndex(0))
					{
						Widgets[0]->RemoveFromParent();
					}
				}, 5.8f, false);
			}
		}
	}
}

bool AInGameHUD::GetKeepGalleryOpen() const
{
	return KeepGalleryOpen;
}

void AInGameHUD::SetKeepGalleryOpen(bool bKeepGalleryOpen)
{
	KeepGalleryOpen = bKeepGalleryOpen;
}

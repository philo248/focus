// Copyright BlindGames. All Rights Reserved.

#pragma once

#include "InstantCameraComponent.h"
#include "CustomGameMode.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Camera/CameraComponent.h"
#include "MainPlayer.h"
#include "InGameHUD.h"
#include "Math/UnrealMathUtility.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "PhotoGalleryGameInstance.h"
#include "PhotoGallery.h"
#include "QuestData.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SpotLightComponent.h"
#include "HAL/FileManagerGeneric.h"

AInstantCameraComponent::AInstantCameraComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Scene Root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComponent;

	// Set Photo mesh component
	PhotoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhotoMesh"));
	PhotoMesh->SetupAttachment(RootSceneComponent);

	// Set Camera mesh component
	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	CameraMesh->SetupAttachment(RootSceneComponent);

	// Set Capture component
	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
	CaptureComponent->SetupAttachment(RootSceneComponent);
	CaptureComponent->bEditableWhenInherited = true;
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;

	// Set camera flash component
	CameraFlash = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flash"));
	CameraFlash->SetupAttachment(CameraMesh);
	
	// Set some parameters for the camera post process settings Blur effect.
	CameraPostProSettings.bOverride_DepthOfFieldFocalDistance = true;
	CameraPostProSettings.bOverride_DepthOfFieldFstop = true;
	CameraPostProSettings.bOverride_DepthOfFieldFocalRegion = true;
	CameraPostProSettings.DepthOfFieldFstop = 1.2f;
	CameraPostProSettings.DepthOfFieldFocalRegion = 50.0f;
	CaptureComponentPostProSettings.bOverride_DepthOfFieldFocalDistance = true;

	// Set timeline reference and delegate
	TimelinePrintPicture = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelinePrint"));
	TimelineDevelopPicture = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineDevelop"));

	PrintPhotoMovement.BindUFunction(this,FName("PrintPhotoTimeline"));
	PrintPhotoTransition.BindUFunction(this,FName("DevelopPhotoTimeline"));

	ZOffset=15.0f; // Default distance to travel of the photo when printing

	DeleteAllPhoto(); // Delete all saved pictures and the old game save before starting new game;
}

void AInstantCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Make the component invisible at start
	PhotoMesh->SetVisibility(false);
	CameraMesh->SetVisibility(false);
	CameraMesh->SetCastShadow(false);
	CameraFlash->SetVisibility(false); // Camera flash is not visible

	// Create and set Dynamic Material instance of the photo
	Material = PhotoMesh->GetMaterial(0);
	DynamicPhotoMaterial = UMaterialInstanceDynamic::Create(Material, this);
	PhotoMesh->SetMaterial(0, DynamicPhotoMaterial);
	
	auto const PlayerRef = Cast<AMainPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	// Get player character components and set references
	if (PlayerRef)
	{
		PlayerCamera = PlayerRef->FindComponentByClass<UCameraComponent>();
		PlayerMesh = PlayerRef->GetMesh();

		GameFOV = PlayerCamera->FieldOfView;
		GamePostProcess = PlayerCamera->PostProcessSettings;
		CaptureComponent->PostProcessSettings = GamePostProcess;
		CameraOriginLocation = PlayerCamera->GetRelativeTransform().GetLocation();
		PhotoOriginInCamera = FVector(0.f,0.f,02.f);

		// Attach capture component to player camera, this will be the view of the actual picture
		CaptureComponent->AttachToComponent(PlayerCamera, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true));
	}

	// Player HUD reference
	GameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	PictureMode = false; // Default picture mode. Camera is not set
	AnimFinish = true; // Reference if an animation is being played
	FlashEnable = false; // Don't enable the flash at the beginning
}

void AInstantCameraComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Set camera post process effect when camera is set
	if (PictureMode)
	{
		setFocalDistance();
	}
}

void AInstantCameraComponent::CheckIfCameraIsSet()
{
	if (AnimFinish)
	{
		if (PictureMode)
		{
			UnsetCamera();
		}
		else
		{
			SetCamera();
		}
	}
	//TODO : Check if camera is full. If camera is full then show message to screen.
}

void AInstantCameraComponent::SetCamera()
{
	auto const PlayerRef = Cast<AMainPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerRef)
	{
		PlayerRef->SetIsInPhotoMode(true);
	}
	
	// Unhide camera mesh and cast shadow
	CameraMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), "CameraSocket_Hold");
	CameraMesh->SetVisibility(true);
	CameraMesh->SetCastShadow(true);

	// Play animation to set camera in frame
	UAnimInstance* AnimInstance = (PlayerMesh) ? PlayerMesh->GetAnimInstance() : nullptr;

	if (SetCameraMontage && AnimInstance)
	{
		float const Duration = AnimInstance->Montage_Play(SetCameraMontage, 1.f);
		if (Duration > 0.f)
		{
			AnimFinish = false;
			AnimInstance->Montage_JumpToSection(FName("SetCamera"), SetCameraMontage);
		}

		// Set timer to wait for the animation
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
			EnablePictureMode();
			}, 0.6f, false);
	}
}

void AInstantCameraComponent::UnsetCamera()
{
	UAnimInstance* AnimInstance = (PlayerMesh) ? PlayerMesh->GetAnimInstance() : nullptr;

	// Play anim montage backward to unset camera
	if (SetCameraMontage && AnimInstance)
	{
		float const Duration = AnimInstance->Montage_Play(SetCameraMontage, -0.9f, EMontagePlayReturnType(), 1.3f);
		if (Duration > 0.f)
		{
			AnimFinish = false;
			AnimInstance->Montage_JumpToSection(FName("SetCamera"), SetCameraMontage);
			DisablePictureMode();
		}

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
			CameraMesh->SetVisibility(false);
			CameraMesh->SetCastShadow(false);
			auto const PlayerRef = Cast<AMainPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
			if (PlayerRef)
			{
				PlayerRef->SetIsInPhotoMode(false);
			}
			AnimFinish = true;
			}, 1.5f, false);
	}
}

void AInstantCameraComponent::EnablePictureMode()
{
	// Change HUD to view finder widget
	if (GameHUD)
	{
		GameHUD->EnableCameraView();
		
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
			AdjustCameraForPictureMode();
			PictureMode = true;
			AnimFinish = true;
			GameHUD->ToggleFlashIcon(FlashEnable);
			}, 0.5f, false);
	}
}

void AInstantCameraComponent::DisablePictureMode()
{
	PictureMode = false;
	
	// change HUD to basic view widget
	if (GameHUD)
	{
		GameHUD->DisableCameraView();
	}

	// Wait the animation to finish before reset camera effect
	FTimerHandle handleCameraEffect;
	GetWorld()->GetTimerManager().SetTimer(handleCameraEffect, [this]() {
		ResetCameraEffects();
		}, 0.4f, false);
}

void AInstantCameraComponent::AdjustCameraForPictureMode()
{
	CaptureComponent->FOVAngle = PlayerCamera->FieldOfView * FieldOfViewConversionFactor;
	PlayerCamera->SetRelativeLocation( FVector(CameraOriginLocation.X,CameraOriginLocation.Y + 25,CameraOriginLocation.Z));
}

void AInstantCameraComponent::ResetCameraEffects()
{
	PlayerCamera->SetFieldOfView(GameFOV);
	PlayerCamera->PostProcessSettings = GamePostProcess;
	PlayerCamera->SetRelativeLocation(CameraOriginLocation);
}

void AInstantCameraComponent::setFocalDistance()
{
	static float CurrentFocalDistance;
	FVector Start = PlayerCamera->GetComponentLocation();
	FVector  End = Start + (PlayerCamera->GetForwardVector() * 5000.0f);
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;

	//DrawDebugLine(GetWorld(),Start, End, FColor::Green, false, 1, 0 ,0); // Debug green line of the line trace. Uncomment if you want to see the line trace.

	bool isHit  = GetWorld()->LineTraceSingleByChannel(OutHit,Start, End, ECC_Visibility, CollisionParams);

	// Check if the line trace exist and if it hits something. Set the distance of the object the line trace is hitting
	if(isHit && OutHit.bBlockingHit)
	{
			// Debug log. Uncomment if you want to see the log of what the line trace touch
			/*
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,1.f,FColor::Red,
	FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));
				GEngine->AddOnScreenDebugMessage(-1,1.f,FColor::Red,
	FString::Printf(TEXT("Impact Point: %s"), *OutHit.ImpactPoint.ToString()));
				GEngine->AddOnScreenDebugMessage(-1,1.f,FColor::Red,
	FString::Printf(TEXT("Normal Point: %s"), *OutHit.ImpactNormal.ToString()));
			}*/
			TargetFocalDistance = OutHit.Distance;
	}
	else
	{
		TargetFocalDistance = 5000.f; // Default focal distance value
	}

	CurrentFocalDistance = FMath::FInterpTo(CurrentFocalDistance, TargetFocalDistance, GetWorld()->DeltaTimeSeconds, 6.0f); // Set focal distance

	// Set blur effet on the camera view
	CameraPostProSettings.DepthOfFieldFocalDistance = CurrentFocalDistance;
	CaptureComponentPostProSettings.DepthOfFieldFocalDistance = CurrentFocalDistance;
	PlayerCamera->PostProcessSettings = CameraPostProSettings;
	CaptureComponent->PostProcessSettings = CaptureComponentPostProSettings;
}

void AInstantCameraComponent::ShootAndReceivePicture()
{
	if (AnimFinish)
	{
		ShowFlash();
		
		// Set timer to be in time with the fade out animation
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
			if (ShootPhotoSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ShootPhotoSound, CameraMesh->GetComponentLocation());
			}
			}, 0.5f, false);
		
		//ingame Screenshot
		CaptureComponent->CaptureScene();

		//Set texture named "Picture" in the materiel to the new screenshot
		DynamicPhotoMaterial->SetTextureParameterValue("Picture", CaptureComponent->TextureTarget);
		DynamicPhotoMaterial->SetScalarParameterValue("Transition", 0.f);
		
		PhotoMesh->SetVisibility(true);
		PhotoMesh->SetCastShadow(true);
		PhotoMesh->AttachToComponent(CameraMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true));
		PhotoMesh->SetRelativeLocation(PhotoOriginInCamera);
	
		int PictureCount = AddPhotoToGameSave();
		const FString FilePath = FPaths::ProjectSavedDir() + "Pictures";
		const FString FileName =  "Pic_" + FString::FromInt( PictureCount ) + ".png";
		UKismetRenderingLibrary::ExportRenderTarget(this, CaptureComponent->TextureTarget, FilePath , FileName);

		// Find all objects include in the photo, check all objects and validate if there one valid distortion object, if there is one add it to success list
		TArray<AActor*> HitActor = FindObjectsInFrame();
		FindObjectWithTag(HitActor, PictureCount);
		
		DisablePictureMode();
		
		UAnimInstance* AnimInstance = (PlayerMesh) ? PlayerMesh->GetAnimInstance() : nullptr;

		// Play anim to print photo and put away the camera
		if (SetCameraMontage && AnimInstance)
		{
			float const Duration = AnimInstance->Montage_Play(SetCameraMontage, 1.f);
			if (Duration > 0.f)
			{
				AnimFinish = false;
				AnimInstance->Montage_JumpToSection(FName("PrintPicture"), SetCameraMontage);
			}
		}

		//Wait for the animation to finish, hide camera and reset camera effect after
		FTimerHandle handle2;
		GetWorld()->GetTimerManager().SetTimer(handle2, [this]() {
			CameraMesh->SetVisibility(false);
			CameraMesh->SetCastShadow(false);
			PhotoMesh->SetVisibility(false);
			PhotoMesh->SetCastShadow(false);
			auto const PlayerRef = Cast<AMainPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
			PlayerRef->SetIsInPhotoMode(false);
			AnimFinish = true;
			}, 4.5f, false);
	}
}

void AInstantCameraComponent::PrintPhoto()
{
	if (CurvePrintPicture)
	{
		TimelinePrintPicture->AddInterpFloat(CurvePrintPicture, PrintPhotoMovement, FName("Alpha"));

		StartLocation = PhotoMesh->GetRelativeTransform().GetLocation();
		EndLocation = FVector(StartLocation.X, StartLocation.Y + ZOffset, StartLocation.Z);

		TimelinePrintPicture->SetLooping(false);
		TimelinePrintPicture->SetIgnoreTimeDilation(true);

		TimelinePrintPicture->PlayFromStart();
	}
}

void AInstantCameraComponent::PrintPhotoTimeline(float Value)
{
	PhotoMesh->SetRelativeLocation(FMath::Lerp(StartLocation, EndLocation, Value));
}

void AInstantCameraComponent::GrabbedPhoto()
{
	//Attach photo on the player hand
	PhotoMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true), "PictureSocket");
	DevelopPhoto();
}

void AInstantCameraComponent::ShowFlash()
{
	if (FlashEnable)
	{
		APlayerCameraManager* CameraManagerRef = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0); // get player camera manager to rotate the flash in the direction of the player camera
		CameraFlash->SetWorldRotation(CameraManagerRef->GetCameraRotation());

		CameraFlash->SetVisibility(true);
		
		//Wait couple of mili seconde before hide the flash to let the time to take the photo
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
			CameraFlash->SetVisibility(false);
			// Attach camera to print socket on player mesh
			CameraMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), "CameraSocket_Print");
			}, 0.3f, false);
	}
	else
	{
		// Attach camera to print socket on player mesh
		CameraMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), "CameraSocket_Print");
	}
}

bool AInstantCameraComponent::ToggleFlash()
{
	if (FlashEnable)
	{
		FlashEnable = false;
	}
	else
	{
		FlashEnable = true;
	}
	
	return FlashEnable;
}

void AInstantCameraComponent::DevelopPhoto()
{
	if (CurvePrintPicture)
	{
		TimelineDevelopPicture->AddInterpFloat(CurvePrintPicture, PrintPhotoTransition, FName("Alpha"));

		TimelineDevelopPicture->SetLooping(false);
		TimelineDevelopPicture->SetIgnoreTimeDilation(true);

		TimelineDevelopPicture->PlayFromStart();
	}
}

void AInstantCameraComponent::DevelopPhotoTimeline(float Value)
{
	// Unhide material texture on the picture
	DynamicPhotoMaterial->SetScalarParameterValue("Transition", Value);
}

int AInstantCameraComponent::AddPhotoToGameSave()
{
	int32 LastIndexNumber = 0;
	
	//Finds and stocks the current GameInstance containing the gamesave
	GameInstanceRef = Cast<UPhotoGalleryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	//Checks if there is a GameInstance
	if(GameInstanceRef  != nullptr)
	{
		bool GameSaveExist = false;
		//Verifies if there is no game save
		if(!GameInstanceRef->IsNewGame())
		{
			//if  no game save exist, create one
			GameSaveExist = GameInstanceRef->CreateNewSaveGame();
		}
		else // If the game save already exist get the current game save
		{
			GameSaveExist = GameInstanceRef->LoadGame();
		}
		
		if (GameSaveExist)
		{
			GameInstanceRef->CurrentSaveGame->PictureCount += 1;

			if (GameInstanceRef->CurrentSaveGame->PictureNumbers.Num() > 0)
			{
				LastIndexNumber = GameInstanceRef->CurrentSaveGame->PictureNumbers.Last();
				LastIndexNumber += 1;
			}
			else
			{
				LastIndexNumber = 1;
			}

			GameInstanceRef->CurrentSaveGame->PictureNumbers.AddUnique(LastIndexNumber);
			
			GameInstanceRef->SaveGame();
		}
	}
	return LastIndexNumber;
}

void AInstantCameraComponent::DeleteAllPhoto()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	const FString Directory = FPaths::ProjectSavedDir();
	const FString Folder = "Pictures";
	const FString CompletePath = Directory + Folder;

	// Verify if the path exist
	if (PlatformFile.DirectoryExists(*CompletePath))
	{
		//PlatformFile.DeleteDirectory(*CompletePath);
		FFileManagerGeneric::Get().DeleteDirectory(*CompletePath, true, true);
	}
	
	//Finds and stocks the current GameInstance containing the gamesave
	GameInstanceRef = Cast<UPhotoGalleryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	//Checks if there is a GameInstance
	if(GameInstanceRef  != nullptr)
	{
		//Verifies if there is no game save
		if(GameInstanceRef->IsNewGame())
		{
			//if  game save exist, load it and if load is successful, delete the game save
			if (GameInstanceRef->LoadGame())
			{
				bool GameDeleted = GameInstanceRef->DeleteGame();
			}
		}
	}
}

TArray<AActor*> AInstantCameraComponent::FindObjectsInFrame()
{
	float XOffset = 0.f;
	float YOffset = 0.f;
	const float XSampleRate = 60.f;
	const float YSampleRate = 40.f;
	const float Xratio = 0.5f;
	TArray<AActor*> HitActorsList;
	
	float XScreenSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld()).X * Xratio;
	float YScreenSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld()).Y;

	const float XOffsetIncrement = XScreenSize / XSampleRate;
	const float YOffsetIncrement = YScreenSize / YSampleRate;

	FVector WorldLocation;
	FVector WorldDirection;
	FVector Start;
	FVector End;
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	
	for (int i = 0; i <= 60; i++)
	{
		if (i > 0)
		{
			XOffset = XOffsetIncrement + XOffset;
			YOffset = 0.f;
		}
		else
		{
			XOffset = (UWidgetLayoutLibrary::GetViewportSize(GetWorld()).X - XScreenSize) / 2;
		}

		for (int j = 0; j <= 40; j++)
		{
			YOffset = YOffsetIncrement + YOffset;
			
			GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(XOffset, YOffset, WorldLocation, WorldDirection);
			Start = WorldLocation;
			End = (WorldDirection * 4000.f) + PlayerCamera->GetComponentLocation();
			
			// DrawDebugLine(GetWorld(),Start, End, FColor::Red, false, 3, 0 ,0); // Debug green line of the line trace. Uncomment if you want to see the line trace.
		
			bool isHit  = GetWorld()->LineTraceSingleByChannel(OutHit,Start, End, ECC_Visibility, CollisionParams);

			// Check if the line trace exist and if it hits something.
			if(isHit && OutHit.bBlockingHit)
			{
				/*// Debug log. Uncomment if you want to see the log of what the line trace touch
				if(GEngine && OutHit.Actor != nullptr)
				{
					GEngine->AddOnScreenDebugMessage(-1,1.f,FColor::Red,
					FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));
				}*/
				
				if (OutHit.Actor != nullptr)
				{
					HitActorsList.Add(Cast<AActor>(OutHit.Actor));
				}
			}
		}
	}
	return HitActorsList;
}

void AInstantCameraComponent::FindObjectWithTag(TArray<AActor*> ActorsList, int32 PhotoNumb)
{
	ACustomGameMode* GameMode = Cast<ACustomGameMode>(GetWorld()->GetAuthGameMode());
	GameInstanceRef = Cast<UPhotoGalleryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	
	bool DialogueAdded = false; // Check if a dialogue text was assigned to the photo
	bool HasQuestTag; // Check if the actor have a quest tag

	if (GameMode  != nullptr) // Just to make sure game mode pointer is valid
	{
		for (AActor* CurrentActor : ActorsList) // Loop through all actors inside the photo
		{
			if (CurrentActor->Tags.Num() > 0)
			{
				for(int i = 0; i < GameMode->Quest->QuestDataList.Num(); i++) // Loop through all quests if the current actor have a tag
				{
					FName QuestTag = GameMode->Quest->QuestDataList[i].DistortionActorTag;
				
					HasQuestTag = CurrentActor->ActorHasTag(QuestTag);

					if (HasQuestTag)
					{
						DialogueAdded = true;
						GameMode->Quest->QuestDataList[i].PicturesNumbRef.Add(PhotoNumb);
						GameMode->SetFoundQuest(i);
						GameHUD->ShowDialogue(QuestTag);
						break;
					}
				}
				
				if (!DialogueAdded)
				{
					DialogueAdded = true;
					GameHUD->ShowDialogue(CurrentActor->Tags[0]);
				}
			}

			if (DialogueAdded)
			{
				break;
			}
		}
	}

	if (!DialogueAdded)
	{
		FText text;
		GameInstanceRef->CurrentSaveGame->DialogueTextID.Add(FText::FromName(" ")); // Empty text when the picture don't have a tag
		GameInstanceRef->SaveGame();
	}
	else
	{
		GameInstanceRef->CurrentSaveGame->DialogueTextID.Add(GameHUD->GetDialogue());
		FString test = GameHUD->GetDialogue().ToString();
		UE_LOG(LogTemp, Log, TEXT("SomeString: %s"), *test);
		GameInstanceRef->SaveGame();
	}
}
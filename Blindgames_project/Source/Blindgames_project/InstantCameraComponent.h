// Copyright BlindGames. All Rights Reserved.

/*
* This is the camera used by the player.
* It handle all the function when the player shoot a picture.
* It also change postprocessing effect when the camera is turn on.
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "InstantCameraComponent.generated.h"

class UPhotoGallery;
class UPhotoGalleryGameInstance;

UCLASS()
class BLINDGAMES_PROJECT_API AInstantCameraComponent : public AActor
{
	GENERATED_BODY()
	
	class UTimelineComponent* TimelinePrintPicture; // Print picture timeline component to hold variable and function of the timleine
	class UTimelineComponent* TimelineDevelopPicture; // Print picture timeline component to hold variable and function of the timleine
	
public:
	AInstantCameraComponent();

	// Default game hud reference
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	class AInGameHUD* GameHUD;

	// default Root Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root")
	class USceneComponent* RootSceneComponent;

	// photo mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* PhotoMesh;

	// camera mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* CameraMesh;

	// photo scene component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CaptureComponent")
	class USceneCaptureComponent2D* CaptureComponent;

	// photo material
	UPROPERTY(EditAnywhere, Category = "Material")
	class UMaterialInterface* Material;
	UPROPERTY(EditAnywhere, Category = "Material")
	class UMaterialInstanceDynamic* DynamicPhotoMaterial;

	// Camera flash component
	UPROPERTY(EditAnywhere, Category = "Flash")
	class USpotLightComponent* CameraFlash;
	
	// player camera ref
	UPROPERTY(EditAnywhere, Category = "Player")
	class UCameraComponent* PlayerCamera;

	// player skeletal mesh ref
	UPROPERTY(EditAnywhere, Category = "Player")
	class USkeletalMeshComponent* PlayerMesh;
	
	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* SetCameraMontage;

	// Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundWave* ShootPhotoSound;

	// Reference of the timeline
    UPROPERTY(EditAnywhere, Category = "Timeline")
    class UCurveFloat* CurvePrintPicture;

	// Delegate print photo movement
	FOnTimelineFloat PrintPhotoMovement{};
	
	// Delegate print photo fade in texture
	FOnTimelineFloat PrintPhotoTransition{};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void SetCamera(); // Show camera

	UFUNCTION()
	void EnablePictureMode(); // Change HUD to view finder widget

	UFUNCTION()
	void DisablePictureMode(); // Change HUD to view finder widget

	UFUNCTION()
	void UnsetCamera(); // Hide camera

	UFUNCTION()
	void AdjustCameraForPictureMode(); // Position the camera in the hand of the player

	UFUNCTION()
	void ResetCameraEffects(); // Remove the blur effect and reset position of the view

	UFUNCTION()
	void setFocalDistance(); // Set camera effect
	
	UFUNCTION()
	void DevelopPhoto(); // develop the photo

	UFUNCTION()
	void PrintPhotoTimeline(float Value); // Print photo timeline delegate function
	
	UFUNCTION()
	void DevelopPhotoTimeline(float Value); // develop photo timeline delegate function

	UFUNCTION()
	int AddPhotoToGameSave(); // Add the photo taken by the player to the game save

	UFUNCTION()
	void DeleteAllPhoto(); // Delete all photo from file and delete game save

	UFUNCTION()
	TArray<AActor*> FindObjectsInFrame(); // Find all objects include in the photo

	UFUNCTION()
	void FindObjectWithTag(TArray<AActor*> ActorsList, int32 PhotoNumb); // Check all objects in the photo and validate if there one valid distortion object, if there is one add it to success list in the game save

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CheckIfCameraIsSet(); // Check if the player have the camera in his hand

	UFUNCTION()
	void ShootAndReceivePicture(); // Start the shooting photo animation

	UFUNCTION()
	void PrintPhoto(); // print the photo
	
	UFUNCTION()
	void GrabbedPhoto(); // grab the photo

	UFUNCTION()
	void ShowFlash(); // Show the camera flash is the flash is enable

	UFUNCTION()
	bool ToggleFlash(); // enable or disable the camera flash
	
private:

	UPROPERTY(EditAnywhere, Category = "Camera")
	bool PictureMode; // Determine if camera is on or off

	UPROPERTY(EditAnywhere, Category = "Camera")
	bool AnimFinish; // Determine if animation is done

	UPROPERTY(EditAnywhere, Category = "Camera")
	float GameFOV; // Default Game FOV

	UPROPERTY(EditAnywhere, Category = "Camera")
	bool FlashEnable; // Variable to see if the flash is enable for the camera
	
	const float FieldOfViewConversionFactor = 0.6f; // Float to adjust field of view of the capture component on the photo
	
	UPROPERTY(EditAnywhere, Category = "PostProcess")
	FPostProcessSettings GamePostProcess; // Use to save the normal game post process settings

	UPROPERTY(EditAnywhere, Category = "PostProcess")
	FPostProcessSettings CameraPostProSettings; // Actual camera post process settings

	UPROPERTY(EditAnywhere, Category = "PostProcess")
	FPostProcessSettings CaptureComponentPostProSettings; // Actual capture component post process settings

	UPROPERTY(EditAnywhere, Category = "Camera")
	FVector CameraOriginLocation; // Camera origin position reference

	UPROPERTY(EditAnywhere, Category = "Camera")
	FVector PhotoOriginInCamera; // Photo origin position reference

	UPROPERTY(EditAnywhere, Category = "Camera")
	float TargetFocalDistance; // The distance of the objet touched by the linetrace
	
	UPROPERTY(EditAnywhere, Category = "Timeline")
	FVector StartLocation; // Start location of the photo for the animation

	UPROPERTY(EditAnywhere, Category = "Timeline")
	FVector EndLocation; // End location of the photo for the animation
	
	UPROPERTY(EditAnywhere, Category = "Timeline")
	float ZOffset; // Distance to travel of the photo for the animation
	
	UPROPERTY(EditAnywhere, Category = "GameSave")
	UPhotoGalleryGameInstance * GameInstanceRef; // Reference to the actual game instance
};

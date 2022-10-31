// Copyright BlindGames. All Rights Reserved.

/*
* Sorting board actor class
* Contain the mesh of the board and each photos placed by the player
* Contain the logic to add new photo on the board
* Contain the logic to detect the player when he's near the board
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Board.generated.h"

UCLASS()
class BLINDGAMES_PROJECT_API ABoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoard();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BoardMesh; // Actual mesh of the board

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PenseePhotoMesh; // Pensee photo mesh

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* EmotionPhotoMesh; // Emotion photo mesh

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SituationPhotoMesh; // Situation photo mesh

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere; // Sphere to detect if the player is in front of the board

public:

	UFUNCTION()
	void GetAndChangePhotoMaterial(int32 PictureNumber, int32 ButtonID);

	UFUNCTION()
	void ChangePenseePhotoMaterial(UTexture2D* PhotoTexture); // Change material of the pensee photo
	
	UFUNCTION()
	void ChangeEmotionPhotoMaterial(UTexture2D* PhotoTexture); // Change material of the Emotion photo
	
	UFUNCTION()
	void ChangeSituationPhotoMaterial(UTexture2D* PhotoTexture); // Change material of the Situation photo

	UFUNCTION()
	void ValidateQuest(int32 QuestNumb); // Set a specific quest to completed in the quest data

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:

	// photo material
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	class UMaterialInterface* Material;

	// Photos Dynamic material
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	class UMaterialInstanceDynamic* DynamicPhotoMaterialPensee;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	class UMaterialInstanceDynamic* DynamicPhotoMaterialSituation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	class UMaterialInstanceDynamic* DynamicPhotoMaterialEmotion;

	// Player reference when he's near the board
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	class AMainPlayer* PlayerRef;
};

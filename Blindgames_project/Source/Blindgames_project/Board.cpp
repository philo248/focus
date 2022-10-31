// Copyright BlindGames. All Rights Reserved.

#include "Board.h"
#include "CPP_GalleryUI.h"
#include "CustomGameMode.h"
#include "InGameHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "MainPlayer.h"

// Sets default values
ABoard::ABoard()
{
	// Actual board mesh
	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	BoardMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Geometry/Meshes/SM_Board_01.SM_Board_01'")).Object);
	SetRootComponent(BoardMesh);

	// Pensee photo mesh placed by the player
	PenseePhotoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PenseeMesh"));
	PenseePhotoMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/InstantPhoto/Mesh/SM_InstantPhoto.SM_InstantPhoto'")).Object);
	PenseePhotoMesh->SetupAttachment(BoardMesh);

	// Emotion photo mesh placed by the player
	EmotionPhotoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EmotionMesh"));
	EmotionPhotoMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/InstantPhoto/Mesh/SM_InstantPhoto.SM_InstantPhoto'")).Object);
	EmotionPhotoMesh->SetupAttachment(BoardMesh);

	// Situation photo mesh placed by the player
	SituationPhotoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SituationMesh"));
	SituationPhotoMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/InstantPhoto/Mesh/SM_InstantPhoto.SM_InstantPhoto'")).Object);
	SituationPhotoMesh->SetupAttachment(BoardMesh);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(BoardMesh); // Sphere used for collision detection of the player
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABoard::OnOverlapBegin);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ABoard::OnOverlapEnd);

	// Create and set Dynamic Material instance for all the photos
	// Photo Pensée material setting
	Material = PenseePhotoMesh->GetMaterial(0);
	DynamicPhotoMaterialPensee = UMaterialInstanceDynamic::Create(Material, this);
	PenseePhotoMesh->SetMaterial(0, DynamicPhotoMaterialPensee);
	// Photo Emotion material setting
	Material = EmotionPhotoMesh->GetMaterial(0);
	DynamicPhotoMaterialEmotion = UMaterialInstanceDynamic::Create(Material, this);
	EmotionPhotoMesh->SetMaterial(0, DynamicPhotoMaterialEmotion);
	// Photo Situation material setting
	Material = SituationPhotoMesh->GetMaterial(0);
	DynamicPhotoMaterialSituation = UMaterialInstanceDynamic::Create(Material, this);
	SituationPhotoMesh->SetMaterial(0, DynamicPhotoMaterialSituation);

	// Hide photos mesh
	SituationPhotoMesh->SetVisibility(false);
	EmotionPhotoMesh->SetVisibility(false);
	PenseePhotoMesh->SetVisibility(false);
}

void ABoard::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerRef = Cast<AMainPlayer>(OtherActor);
	
	if (PlayerRef && !PlayerRef->GetIsInPhotoMode())
	{
		PlayerRef->SetCanOpenBoard(true);
		PlayerRef->GameHUD->AccessBoardView();
	}
}

void ABoard::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (PlayerRef && PlayerRef->GetCanOpenBoard())
	{
		if (PlayerRef->GameHUD->isSorting)
		{
			PlayerRef->SetCanOpenBoard(false);
		}
		else
		{
			PlayerRef->SetCanOpenBoard(false);
			PlayerRef->GameHUD->EndAccessBoardView();	
		}
	}
}

void ABoard::GetAndChangePhotoMaterial(int32 PictureNumber, int32 ButtonID)
{
	//Gets the FilePath to the save Picture and its name
	const FString FilePath = FPaths::ProjectSavedDir() + "Pictures";
	FString FileName =  "/Pic_" + FString::FromInt( PictureNumber ) + ".png";

	UCPP_GalleryUI* GalleryUIRef = Cast<UCPP_GalleryUI>(PlayerRef->GameHUD->CurrentWidget);

	if (GalleryUIRef)
	{
		ValidateQuest(ButtonID);
		
		UTexture2D * TexturePhoto = GalleryUIRef->GetTexture(FilePath + FileName);
		 
		// Switch to find wich quest is selected, Change the photo material of the actual quest
		switch (ButtonID)
		{
			case 0: ChangeSituationPhotoMaterial(TexturePhoto);
				break;
			case 1: ChangeEmotionPhotoMaterial(TexturePhoto);
				break;
			case 2: ChangePenseePhotoMaterial(TexturePhoto);
				break;
			default:
				break;
		}

		GalleryUIRef->DelFocusPhoto();
	}
}

void ABoard::ChangePenseePhotoMaterial(UTexture2D* PhotoTexture)
{
	//Set texture named "Picture" in the materiel to the new screenshot
	DynamicPhotoMaterialPensee->SetTextureParameterValue("Picture", PhotoTexture);
	DynamicPhotoMaterialPensee->SetScalarParameterValue("Transition", 1.f);
	PenseePhotoMesh->SetVisibility(true); // Unhide photo mesh
}

void ABoard::ChangeEmotionPhotoMaterial(UTexture2D* PhotoTexture)
{
	//Set texture named "Picture" in the materiel to the new screenshot
	DynamicPhotoMaterialEmotion->SetTextureParameterValue("Picture", PhotoTexture);
	DynamicPhotoMaterialEmotion->SetScalarParameterValue("Transition", 1.f);
	EmotionPhotoMesh->SetVisibility(true); // Unhide photo mesh
}

void ABoard::ChangeSituationPhotoMaterial(UTexture2D* PhotoTexture)
{
	//Set texture named "Picture" in the materiel to the new screenshot
	DynamicPhotoMaterialSituation->SetTextureParameterValue("Picture", PhotoTexture);
	DynamicPhotoMaterialSituation->SetScalarParameterValue("Transition", 1.f);
	SituationPhotoMesh->SetVisibility(true); // Unhide photo mesh
}

void ABoard::ValidateQuest(int32 QuestNumb)
{
	ACustomGameMode* GameMode = Cast<ACustomGameMode>(GetWorld()->GetAuthGameMode());

	GameMode->CompletQuest(QuestNumb); // Call function in game mode to complet the quest at this position in the array
}

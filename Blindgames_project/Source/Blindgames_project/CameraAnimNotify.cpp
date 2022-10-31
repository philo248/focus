// Copyright BlindGames. All Rights Reserved.

#include "CameraAnimNotify.h"
#include "InstantCameraComponent.h"
#include "MainPlayer.h"

void UCameraAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

    // check if the source of the notify is the player
    if (AActor* Actor = MeshComp->GetOwner())
    {
        AMainPlayer* Player = Cast<AMainPlayer>(Actor);

        if (Player)
        {
            // Check the notify param and execute the function in the camera
            if (Param == TEXT("PrintPhoto"))
            {
                Player->CameraRef->PrintPhoto();
            }
            
            if (Param == TEXT("GrabbedPhoto"))
            {
                Player->CameraRef->GrabbedPhoto();
            }
        }
    }   
}
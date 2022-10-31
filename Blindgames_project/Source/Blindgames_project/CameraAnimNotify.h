// Copyright BlindGames. All Rights Reserved.

/*
 * Handle all the animation notifications for the player.
 * Call specific function based on the message sent by the notification.
 */

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CameraAnimNotify.generated.h"

UCLASS()
class BLINDGAMES_PROJECT_API UCameraAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

protected: 

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere)
	FString Param;

};

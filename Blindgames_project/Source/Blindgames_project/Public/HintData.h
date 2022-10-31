// Copyright BlindGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HintData.generated.h"


USTRUCT(BlueprintType)
struct FHint
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName HintText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	bool Active = false;
};

UCLASS(BlueprintType)
class BLINDGAMES_PROJECT_API UHintData : public UDataAsset
{
	GENERATED_BODY()

	public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TArray<FHint> HintDataList;
	
};
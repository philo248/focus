// Copyright BlindGames. All Rights Reserved.

/*
* Base class for User Widget
* Contain all features used by all widgets in the game
*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "HintData.h"
#include "UIUserWidget.generated.h"

USTRUCT(BlueprintType)
struct FDialogue : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	int32 Dialogue_ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 Line_ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText Dialogue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TArray<FHint> HintArray;
};

/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API UUIUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct();
	
protected:

	// Default game hud reference
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	class AInGameHUD* GameHUD;
};

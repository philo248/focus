// Copyright BlindGames. All Rights Reserved.

/*
* Dialogue User Widget
* Contain a Text box to write the actual dialogue to display
*/

#pragma once

#include "CoreMinimal.h"
#include "UIUserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "UIDialogue.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class BLINDGAMES_PROJECT_API UUIDialogue : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	void ChangeTextByID(FName ID);

	UFUNCTION()
	FText GetDialogue()
	{
		return DialogueTxt->Text;
	}

	// Dialogue DataTable Reference, set in blueprint, contain all dialogue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* PhrasesTable;
	
	// Text bloc that contain the dialogue
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DialogueTxt;

	// Background image of the widget
	UPROPERTY(meta = (BindWidget))
	UImage* Backgrnd;
};

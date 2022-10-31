// Copyright BlindGames. All Rights Reserved.

#include "UIDialogue.h"
#include "Components/TextBlock.h"

void UUIDialogue::ChangeTextByID(FName ID)
{
	if (PhrasesTable)
	{
		FDialogue* DialogueRow = PhrasesTable->FindRow<FDialogue>(ID, "DialogueTableContext" , true); // Get the text in the dialog table
		
		if (DialogueRow)
		{
			DialogueTxt->Text = DialogueRow->Dialogue; // Set the text in the UI
		}
	}
}

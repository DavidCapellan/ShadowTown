#include "ShadowDialogueWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UShadowDialogueWidget::ShowLine(const FText& LineText, const FString& SpeakerName)
{
	if (DialogueText)
	{
		DialogueText->SetText(LineText);
	}
	if (SpeakerText)
	{
		SpeakerText->SetText(FText::FromString(SpeakerName));
	}
	if (DialoguePanel)
	{
		DialoguePanel->SetVisibility(ESlateVisibility::Visible);
	}
	SetVisibility(ESlateVisibility::Visible);
	OnLineShown(LineText, SpeakerName);
}

void UShadowDialogueWidget::HideDialogue()
{
	if (DialoguePanel)
	{
		DialoguePanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShadowDialogueWidget.generated.h"

class UTextBlock;
class UBorder;

/** Base widget for in-world dialogue. Subclass as WBP_DialogueHUD in editor. */
UCLASS(Abstract, Blueprintable)
class SHADOWTOWN_API UShadowDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ShowLine(const FText& LineText, const FString& SpeakerName);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void HideDialogue();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnLineShown(const FText& LineText, const FString& SpeakerName);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DialogueText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SpeakerText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> DialoguePanel;
};

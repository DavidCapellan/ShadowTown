#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ShadowDialogueTypes.h"
#include "ShadowDialogueSubsystem.generated.h"

class ACharacter;
class UAnimSequence;
class UAnimationAsset;
class USoundBase;
class UShadowDialogueWidget;

UCLASS()
class SHADOWTOWN_API UShadowDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnShadowDialogueLineChanged OnLineChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnShadowDialogueEnded OnDialogueEnded;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool IsDialogueActive() const { return bDialogueActive; }

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(
		ACharacter* PlayerCharacter,
		AActor* NpcActor,
		const TArray<FShadowDialogueRow>& Lines,
		UAnimSequence* NpcTalkAnimation = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool AdvanceDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SilenceNpc();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetDialogueWidgetClass(TSubclassOf<UShadowDialogueWidget> InWidgetClass);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetBubbleWidgetClass(TSubclassOf<UShadowDialogueWidget> InWidgetClass);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<FShadowDialogueRow> FilterLinesFromTable(
		UDataTable* Table,
		EShadowDialogueLineType LineType,
		const FString& GenderFilter) const;

protected:
	void AdvanceDialogueFromInput();

	void ApplyPlayerInputLock(bool bLock);
	void PlayNpcTalkAnimation(UAnimSequence* TalkAnimation);
	void StopNpcTalkAnimation();
	void ShowCurrentLine();
	UShadowDialogueWidget* GetOrCreateWidget();

	UPROPERTY()
	TObjectPtr<ACharacter> ActivePlayer;

	UPROPERTY()
	TObjectPtr<AActor> ActiveNpc;

	UPROPERTY()
	TArray<FShadowDialogueRow> ActiveLines;

	UPROPERTY()
	TObjectPtr<UAnimSequence> ActiveTalkAnimation;

	UPROPERTY()
	TObjectPtr<UAnimationAsset> PreviousNpcAnimation;

	UPROPERTY()
	TObjectPtr<UShadowDialogueWidget> DialogueWidget;

	UPROPERTY()
	TSubclassOf<UShadowDialogueWidget> DialogueWidgetClass;

	UPROPERTY()
	TSubclassOf<UShadowDialogueWidget> BubbleWidgetClass;

	UPROPERTY()
	TObjectPtr<UShadowDialogueWidget> BubbleWidget;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveAudio;

	int32 CurrentLineIndex = 0;
	bool bDialogueActive = false;
	bool bHadMoveInputIgnored = false;
	int32 AdvanceDialogueBindingHandle = INDEX_NONE;
	bool bAdvanceBindingAdded = false;
};

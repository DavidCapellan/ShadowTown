#include "ShadowDialogueBlueprintLibrary.h"
#include "ShadowDialogueSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Character.h"

void UShadowDialogueBlueprintLibrary::StartNpcDialogue(
	const UObject* WorldContextObject,
	ACharacter* PlayerCharacter,
	AActor* NpcActor,
	UDataTable* DialogueTable,
	EShadowDialogueLineType LineType,
	const FString& NpcGender,
	UAnimSequence* TalkAnimation)
{
	if (!WorldContextObject || !PlayerCharacter || !NpcActor || !DialogueTable)
	{
		return;
	}

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GI = World->GetGameInstance();
	if (!GI)
	{
		return;
	}

	UShadowDialogueSubsystem* DialogueSubsystem = GI->GetSubsystem<UShadowDialogueSubsystem>();
	if (!DialogueSubsystem || DialogueSubsystem->IsDialogueActive())
	{
		return;
	}

	const TArray<FShadowDialogueRow> Lines = DialogueSubsystem->FilterLinesFromTable(
		DialogueTable,
		LineType,
		NpcGender);

	if (Lines.Num() > 0)
	{
		DialogueSubsystem->StartDialogue(PlayerCharacter, NpcActor, Lines, TalkAnimation);
	}
}

void UShadowDialogueBlueprintLibrary::AdvanceActiveDialogue(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return;
	}

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World || !World->GetGameInstance())
	{
		return;
	}

	if (UShadowDialogueSubsystem* DialogueSubsystem = World->GetGameInstance()->GetSubsystem<UShadowDialogueSubsystem>())
	{
		DialogueSubsystem->AdvanceDialogue();
	}
}

bool UShadowDialogueBlueprintLibrary::IsDialogueActive(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return false;
	}

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World || !World->GetGameInstance())
	{
		return false;
	}

	if (const UShadowDialogueSubsystem* DialogueSubsystem = World->GetGameInstance()->GetSubsystem<UShadowDialogueSubsystem>())
	{
		return DialogueSubsystem->IsDialogueActive();
	}

	return false;
}

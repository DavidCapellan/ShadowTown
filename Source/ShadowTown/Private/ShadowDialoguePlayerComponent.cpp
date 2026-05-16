#include "ShadowDialoguePlayerComponent.h"
#include "ShadowDialogueSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/GameInstance.h"

UShadowDialoguePlayerComponent::UShadowDialoguePlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UShadowDialoguePlayerComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (!PlayerInputComponent)
	{
		return;
	}

	PlayerInputComponent->BindAction(TEXT("AdvanceDialogue"), IE_Pressed, this, &UShadowDialoguePlayerComponent::OnAdvancePressed);
}

void UShadowDialoguePlayerComponent::OnAdvancePressed()
{
	AdvanceDialogue();
}

void UShadowDialoguePlayerComponent::AdvanceDialogue()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Pawn->IsPlayerControlled())
	{
		return;
	}

	UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	if (!GI)
	{
		return;
	}

	if (UShadowDialogueSubsystem* DialogueSubsystem = GI->GetSubsystem<UShadowDialogueSubsystem>())
	{
		if (DialogueSubsystem->IsDialogueActive())
		{
			DialogueSubsystem->AdvanceDialogue();
		}
	}
}

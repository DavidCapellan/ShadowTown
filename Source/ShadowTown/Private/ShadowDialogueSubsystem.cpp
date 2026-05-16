#include "ShadowDialogueSubsystem.h"
#include "ShadowDialogueWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/AudioComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimationAsset.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

void UShadowDialogueSubsystem::SetDialogueWidgetClass(TSubclassOf<UShadowDialogueWidget> InWidgetClass)
{
	DialogueWidgetClass = InWidgetClass;
}

void UShadowDialogueSubsystem::SetBubbleWidgetClass(TSubclassOf<UShadowDialogueWidget> InWidgetClass)
{
	BubbleWidgetClass = InWidgetClass;
}

void UShadowDialogueSubsystem::SilenceNpc()
{
	StopNpcTalkAnimation();
	if (ActiveAudio && ActiveAudio->IsPlaying())
	{
		ActiveAudio->Stop();
	}
	
	if (DialogueWidget)
	{
		DialogueWidget->HideDialogue();
	}
	
	if (BubbleWidget)
	{
		BubbleWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

TArray<FShadowDialogueRow> UShadowDialogueSubsystem::FilterLinesFromTable(
	UDataTable* Table,
	EShadowDialogueLineType LineType,
	const FString& GenderFilter) const
{
	TArray<FShadowDialogueRow> Result;
	if (!Table)
	{
		return Result;
	}

	const FString TypeString = StaticEnum<EShadowDialogueLineType>()->GetNameStringByValue(static_cast<int64>(LineType));
	TArray<FShadowDialogueRow*> Rows;
	Table->GetAllRows<FShadowDialogueRow>(TEXT("FilterLinesFromTable"), Rows);
	for (FShadowDialogueRow* Row : Rows)
	{
		if (!Row)
		{
			continue;
		}
		const FString RowType = StaticEnum<EShadowDialogueLineType>()->GetNameStringByValue(static_cast<int64>(Row->Type));
		if (!RowType.Equals(TypeString, ESearchCase::IgnoreCase))
		{
			continue;
		}
		if (!GenderFilter.IsEmpty() && !Row->Gender.Equals(GenderFilter, ESearchCase::IgnoreCase))
		{
			continue;
		}
		Result.Add(*Row);
	}
	return Result;
}

void UShadowDialogueSubsystem::AdvanceDialogueFromInput()
{
	AdvanceDialogue();
}

void UShadowDialogueSubsystem::StartDialogue(
	ACharacter* PlayerCharacter,
	AActor* NpcActor,
	const TArray<FShadowDialogueRow>& Lines,
	UAnimSequence* NpcTalkAnimation)
{
	if (bDialogueActive || !PlayerCharacter || !NpcActor || Lines.Num() == 0)
	{
		return;
	}

	ActivePlayer = PlayerCharacter;
	ActiveNpc = NpcActor;
	ActiveLines = Lines;
	ActiveTalkAnimation = NpcTalkAnimation;
	CurrentLineIndex = 0;
	bDialogueActive = true;

	ApplyPlayerInputLock(true);
	PlayNpcTalkAnimation(NpcTalkAnimation);

	if (APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController()))
	{
		if (UInputComponent* Input = PC->InputComponent)
		{
			FInputActionBinding& Binding = Input->BindAction(TEXT("AdvanceDialogue"), IE_Pressed, this, &UShadowDialogueSubsystem::AdvanceDialogueFromInput);
			Binding.bExecuteWhenPaused = false;
			AdvanceDialogueBindingHandle = Binding.GetHandle();
			bAdvanceBindingAdded = true;
		}
	}

	ShowCurrentLine();
}

bool UShadowDialogueSubsystem::AdvanceDialogue()
{
	if (!bDialogueActive)
	{
		return false;
	}

	++CurrentLineIndex;
	if (CurrentLineIndex >= ActiveLines.Num())
	{
		EndDialogue();
		return false;
	}

	ShowCurrentLine();
	return true;
}

void UShadowDialogueSubsystem::EndDialogue()
{
	if (!bDialogueActive)
	{
		return;
	}

	StopNpcTalkAnimation();
	ApplyPlayerInputLock(false);

	if (bAdvanceBindingAdded)
	{
		if (ActivePlayer)
		{
			if (APlayerController* PC = Cast<APlayerController>(ActivePlayer->GetController()))
			{
				if (UInputComponent* Input = PC->InputComponent)
				{
					Input->RemoveActionBindingForHandle(AdvanceDialogueBindingHandle);
				}
			}
		}
		bAdvanceBindingAdded = false;
		AdvanceDialogueBindingHandle = INDEX_NONE;
	}

	if (UShadowDialogueWidget* Widget = GetOrCreateWidget())
	{
		Widget->HideDialogue();
	}

	bDialogueActive = false;
	CurrentLineIndex = INDEX_NONE;
	ActiveLines.Reset();
	ActivePlayer = nullptr;
	ActiveNpc = nullptr;
	ActiveTalkAnimation = nullptr;

	OnDialogueEnded.Broadcast();
}

void UShadowDialogueSubsystem::ApplyPlayerInputLock(bool bLock)
{
	if (!ActivePlayer)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(ActivePlayer->GetController());
	if (PC)
	{
		if (bLock)
		{
			ActivePlayer->DisableInput(PC);
			PC->SetIgnoreMoveInput(true);
		}
		else
		{
			ActivePlayer->EnableInput(PC);
			PC->SetIgnoreMoveInput(false);
		}
		bHadMoveInputIgnored = bLock;
	}

	if (UCharacterMovementComponent* MoveComp = ActivePlayer->GetCharacterMovement())
	{
		if (bLock)
		{
			MoveComp->StopMovementImmediately();
			MoveComp->DisableMovement();
		}
		else
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}
	}
}

void UShadowDialogueSubsystem::PlayNpcTalkAnimation(UAnimSequence* TalkAnimation)
{
	if (!ActiveNpc || !TalkAnimation)
	{
		return;
	}

	USkeletalMeshComponent* Mesh = ActiveNpc->FindComponentByClass<USkeletalMeshComponent>();
	if (!Mesh)
	{
		return;
	}

	PreviousNpcAnimation = Mesh->AnimationData.AnimToPlay;
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->PlayAnimation(TalkAnimation, true);
}

void UShadowDialogueSubsystem::StopNpcTalkAnimation()
{
	if (!ActiveNpc)
	{
		return;
	}

	if (USkeletalMeshComponent* Mesh = ActiveNpc->FindComponentByClass<USkeletalMeshComponent>())
	{
		Mesh->Stop();
		if (PreviousNpcAnimation)
		{
			Mesh->PlayAnimation(PreviousNpcAnimation, true);
		}
	}
	PreviousNpcAnimation = nullptr;
}

void UShadowDialogueSubsystem::ShowCurrentLine()
{
	if (!ActiveLines.IsValidIndex(CurrentLineIndex))
	{
		return;
	}

	const FShadowDialogueRow& Line = ActiveLines[CurrentLineIndex];
	OnLineChanged.Broadcast(Line.Text, Line.CharacterName);

	if (UShadowDialogueWidget* Widget = GetOrCreateWidget())
	{
		Widget->ShowLine(Line.Text, Line.CharacterName);
	}

	if (BubbleWidgetClass && ActiveNpc)
	{
		if (!BubbleWidget)
		{
			APlayerController* PC = Cast<APlayerController>(ActivePlayer->GetController());
			if (PC)
			{
				BubbleWidget = CreateWidget<UShadowDialogueWidget>(PC, BubbleWidgetClass);
				if (BubbleWidget)
				{
					BubbleWidget->AddToViewport(50);
				}
			}
		}

		if (BubbleWidget)
		{
			BubbleWidget->SetVisibility(ESlateVisibility::Visible);
			BubbleWidget->ShowLine(Line.Text, Line.CharacterName);
			// In a real implementation, we'd update its position per tick or use a WidgetComponent
			// For now, we'll assume it's set up to follow or just show in a fixed cloud area
		}
	}

	if (Line.AudioAsset && ActivePlayer)
	{
		if (ActiveAudio && ActiveAudio->IsPlaying())
		{
			ActiveAudio->Stop();
		}
		ActiveAudio = UGameplayStatics::SpawnSound2D(ActivePlayer, Line.AudioAsset);
	}
}

UShadowDialogueWidget* UShadowDialogueSubsystem::GetOrCreateWidget()
{
	if (DialogueWidget)
	{
		return DialogueWidget;
	}

	if (!DialogueWidgetClass || !ActivePlayer)
	{
		return nullptr;
	}

	APlayerController* PC = Cast<APlayerController>(ActivePlayer->GetController());
	if (!PC)
	{
		return nullptr;
	}

	DialogueWidget = CreateWidget<UShadowDialogueWidget>(PC, DialogueWidgetClass);
	if (DialogueWidget)
	{
		DialogueWidget->AddToViewport(100);
	}
	return DialogueWidget;
}

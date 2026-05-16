#include "ShadowDialogueNpcComponent.h"
#include "ShadowDialogueSubsystem.h"
#include "GameFramework/Character.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UShadowDialogueNpcComponent::UShadowDialogueNpcComponent()
{
	InitSphereRadius(220.f);
	SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SetGenerateOverlapEvents(true);
}

void UShadowDialogueNpcComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UShadowDialogueNpcComponent::OnDialogueSphereBeginOverlap);
}

void UShadowDialogueNpcComponent::OnDialogueSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bAutoStartOnOverlap && !bRequireInteractKey)
	{
		TryStartDialogue(OtherActor);
	}
}

bool UShadowDialogueNpcComponent::IsPlayerActor(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	const APawn* Pawn = Cast<APawn>(Actor);
	return Pawn && Pawn->IsPlayerControlled();
}

void UShadowDialogueNpcComponent::TryStartDialogue(AActor* OtherActor)
{
	if (!IsPlayerActor(OtherActor))
	{
		return;
	}

	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (!PlayerCharacter || !DialogueTable)
	{
		return;
	}

	UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
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
		DialogueType,
		NpcGender);

	if (Lines.Num() == 0)
	{
		return;
	}

	DialogueSubsystem->StartDialogue(PlayerCharacter, GetOwner(), Lines, TalkAnimation);
}

void UShadowDialogueNpcComponent::TryAdvanceDialogue(AActor* InstigatorActor)
{
	if (!IsPlayerActor(InstigatorActor))
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

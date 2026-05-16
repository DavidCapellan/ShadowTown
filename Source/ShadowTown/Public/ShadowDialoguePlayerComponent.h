#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShadowDialoguePlayerComponent.generated.h"

/** Handles advancing dialogue while movement is locked on the player pawn. */
UCLASS(ClassGroup = (ShadowTown), meta = (BlueprintSpawnableComponent))
class SHADOWTOWN_API UShadowDialoguePlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShadowDialoguePlayerComponent();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();

protected:
	void OnAdvancePressed();
};

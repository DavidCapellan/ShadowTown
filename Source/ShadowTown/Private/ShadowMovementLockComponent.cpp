#include "ShadowMovementLockComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UShadowMovementLockComponent::UShadowMovementLockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.f;
}

void UShadowMovementLockComponent::SetMovementLocked(bool bLocked)
{
	bMovementLocked = bLocked;

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			if (bLocked)
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
}

void UShadowMovementLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bMovementLocked)
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
			if (MoveComp->MovementMode != MOVE_None)
			{
				MoveComp->DisableMovement();
			}
		}
	}
}

#include "ShadowPlayerInteractionLibrary.h"
#include "ShadowInteractionMenuWidget.h"
#include "ShadowMovementLockComponent.h"
#include "ShadowDialogueSubsystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"

static TWeakObjectPtr<UShadowInteractionMenuWidget> GActiveMenuWidget;
static TWeakObjectPtr<ACharacter> GLockedPlayer;

void UShadowPlayerInteractionLibrary::LockPlayerMovementForMenu(ACharacter* PlayerCharacter)
{
	if (!PlayerCharacter)
	{
		return;
	}

	GLockedPlayer = PlayerCharacter;

	if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	if (APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController()))
	{
		PlayerCharacter->DisableInput(PC);
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(false);
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());
	}

	if (UShadowMovementLockComponent* LockComp = PlayerCharacter->FindComponentByClass<UShadowMovementLockComponent>())
	{
		LockComp->SetMovementLocked(true);
	}
}

void UShadowPlayerInteractionLibrary::UnlockPlayerMovementForMenu(ACharacter* PlayerCharacter)
{
	if (!PlayerCharacter)
	{
		return;
	}

	if (GLockedPlayer.Get() == PlayerCharacter)
	{
		GLockedPlayer.Reset();
	}

	// Si hay un diálogo activo, terminarlo también
	if (UGameInstance* GI = PlayerCharacter->GetGameInstance())
	{
		if (UShadowDialogueSubsystem* DialogueSub = GI->GetSubsystem<UShadowDialogueSubsystem>())
		{
			if (DialogueSub->IsDialogueActive())
			{
				DialogueSub->EndDialogue();
			}
		}
	}

	if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	if (APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController()))
	{
		PlayerCharacter->EnableInput(PC);
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());

		// Restaurar la cámara al jugador (perspectiva tercera persona normal)
		PC->SetViewTargetWithBlend(PlayerCharacter, 0.4f, VTBlend_Cubic);
	}

	if (UShadowMovementLockComponent* LockComp = PlayerCharacter->FindComponentByClass<UShadowMovementLockComponent>())
	{
		LockComp->SetMovementLocked(false);
	}
}

bool UShadowPlayerInteractionLibrary::IsPlayerMovementLockedForMenu(ACharacter* PlayerCharacter)
{
	return GLockedPlayer.IsValid() && GLockedPlayer.Get() == PlayerCharacter;
}

static double GLastMenuCloseTime = 0.0;

UShadowInteractionMenuWidget* UShadowPlayerInteractionLibrary::ShowInteractionMenu(
	ACharacter* PlayerCharacter,
	AActor* NpcActor,
	TSubclassOf<UShadowInteractionMenuWidget> MenuClass)
{
	if (!PlayerCharacter)
	{
		return nullptr;
	}

	// Prevent instant reopening (e.g. if tied to continuous overlap or tick)
	if (GActiveMenuWidget.IsValid())
	{
		return GActiveMenuWidget.Get();
	}

	// 1.5 seconds cooldown after closing the menu to prevent infinite loop
	if (FPlatformTime::Seconds() - GLastMenuCloseTime < 1.5)
	{
		return nullptr;
	}

	LockPlayerMovementForMenu(PlayerCharacter);

	APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
	if (!PC)
	{
		UnlockPlayerMovementForMenu(PlayerCharacter);
		return nullptr;
	}

	UClass* WidgetClass = MenuClass ? MenuClass.Get() : UShadowInteractionMenuWidget::StaticClass();
	UShadowInteractionMenuWidget* Menu = CreateWidget<UShadowInteractionMenuWidget>(PC, WidgetClass);
	if (!Menu)
	{
		UnlockPlayerMovementForMenu(PlayerCharacter);
		return nullptr;
	}

	Menu->AddToViewport(300);
	Menu->ShowMenu(PlayerCharacter, NpcActor);
	GActiveMenuWidget = Menu;
	return Menu;
}

void UShadowPlayerInteractionLibrary::HideInteractionMenu(ACharacter* PlayerCharacter)
{
	if (GActiveMenuWidget.IsValid())
	{
		GActiveMenuWidget->HideMenu();
		GActiveMenuWidget->RemoveFromParent();
		GActiveMenuWidget.Reset();
		GLastMenuCloseTime = FPlatformTime::Seconds();
	}

	if (PlayerCharacter)
	{
		UnlockPlayerMovementForMenu(PlayerCharacter);
	}
}

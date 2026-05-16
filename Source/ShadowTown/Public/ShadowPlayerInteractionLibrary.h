#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShadowPlayerInteractionLibrary.generated.h"

class ACharacter;
class UShadowInteractionMenuWidget;

UCLASS()
class SHADOWTOWN_API UShadowPlayerInteractionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ShadowTown|Interaction", meta = (WorldContext = "WorldContextObject"))
	static void LockPlayerMovementForMenu(ACharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "ShadowTown|Interaction", meta = (WorldContext = "WorldContextObject"))
	static void UnlockPlayerMovementForMenu(ACharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "ShadowTown|Interaction", meta = (WorldContext = "WorldContextObject"))
	static bool IsPlayerMovementLockedForMenu(ACharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "ShadowTown|Interaction", meta = (WorldContext = "WorldContextObject"))
	static UShadowInteractionMenuWidget* ShowInteractionMenu(
		ACharacter* PlayerCharacter,
		AActor* NpcActor,
		TSubclassOf<UShadowInteractionMenuWidget> MenuClass);

	UFUNCTION(BlueprintCallable, Category = "ShadowTown|Interaction", meta = (WorldContext = "WorldContextObject"))
	static void HideInteractionMenu(ACharacter* PlayerCharacter);
};

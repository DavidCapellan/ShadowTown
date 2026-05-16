#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShadowMovementLockComponent.generated.h"

/** Refuerza el bloqueo de WASD mientras el menú de interacción está abierto. */
UCLASS(ClassGroup = (ShadowTown), meta = (BlueprintSpawnableComponent))
class SHADOWTOWN_API UShadowMovementLockComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShadowMovementLockComponent();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetMovementLocked(bool bLocked);

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsMovementLocked() const { return bMovementLocked; }

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	bool bMovementLocked = false;
};

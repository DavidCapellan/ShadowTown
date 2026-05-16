#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "ShadowDialogueTypes.h"
#include "ShadowDialogueNpcComponent.generated.h"

class UDataTable;
class UAnimSequence;

UCLASS(ClassGroup = (ShadowTown), meta = (BlueprintSpawnableComponent))
class SHADOWTOWN_API UShadowDialogueNpcComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UShadowDialogueNpcComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<UDataTable> DialogueTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	EShadowDialogueLineType DialogueType = EShadowDialogueLineType::PreCombate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString NpcGender = TEXT("Female");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString NpcDisplayName = TEXT("Sombra");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<UAnimSequence> TalkAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<UAnimSequence> IdleAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bAutoStartOnOverlap = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bRequireInteractKey = false;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void TryStartDialogue(AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void TryAdvanceDialogue(AActor* InstigatorActor);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDialogueSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	bool IsPlayerActor(AActor* Actor) const;
};

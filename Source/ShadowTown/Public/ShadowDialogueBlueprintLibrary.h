#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShadowDialogueTypes.h"
#include "ShadowDialogueBlueprintLibrary.generated.h"

class ACharacter;
class UAnimSequence;
class UDataTable;

UCLASS()
class SHADOWTOWN_API UShadowDialogueBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dialogue", meta = (WorldContext = "WorldContextObject"))
	static void StartNpcDialogue(
		const UObject* WorldContextObject,
		ACharacter* PlayerCharacter,
		AActor* NpcActor,
		UDataTable* DialogueTable,
		EShadowDialogueLineType LineType,
		const FString& NpcGender,
		UAnimSequence* TalkAnimation);

	UFUNCTION(BlueprintCallable, Category = "Dialogue", meta = (WorldContext = "WorldContextObject"))
	static void AdvanceActiveDialogue(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Dialogue", meta = (WorldContext = "WorldContextObject"))
	static bool IsDialogueActive(const UObject* WorldContextObject);
};

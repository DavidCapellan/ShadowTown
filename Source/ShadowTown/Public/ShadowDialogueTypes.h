#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "ShadowDialogueTypes.generated.h"

UENUM(BlueprintType)
enum class EShadowDialogueLineType : uint8
{
	PreCombate UMETA(DisplayName = "PreCombate"),
	PlayerChallenge UMETA(DisplayName = "PlayerChallenge"),
	DerrotaNPC UMETA(DisplayName = "DerrotaNPC"),
	VictoriaNPC UMETA(DisplayName = "VictoriaNPC")
};

USTRUCT(BlueprintType)
struct FShadowDialogueRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	EShadowDialogueLineType Type = EShadowDialogueLineType::PreCombate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FString Gender;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<USoundBase> AudioAsset = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShadowDialogueLineChanged, FText, LineText, const FString&, SpeakerName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShadowDialogueEnded);

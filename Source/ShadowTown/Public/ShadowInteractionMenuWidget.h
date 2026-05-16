#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShadowInteractionMenuWidget.generated.h"

class UButton;
class UTextBlock;
class UCanvasPanel;
class UOverlay;
class UBorder;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionMenuChoice);

/** Menú de elección (Responder / Ignorar / Salir) anclado a pantalla. */
UCLASS()
class SHADOWTOWN_API UShadowInteractionMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionMenuChoice OnRespond;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionMenuChoice OnIgnore;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionMenuChoice OnExit;

	// Eventos implementables en Blueprint para añadir lógica visual
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnRespondClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnIgnoreClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnExitClicked();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ShowMenu(ACharacter* PlayerCharacter, AActor* NpcActor);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HideMenu();

protected:
	virtual void NativeConstruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UPROPERTY()
	TObjectPtr<UCanvasPanel> RootCanvas;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BtnResponder;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BtnIgnorar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BtnSalir;

	UPROPERTY()
	TWeakObjectPtr<ACharacter> CachedPlayer;

	UPROPERTY()
	TWeakObjectPtr<AActor> CachedNpc;

	bool bUiBuilt = false;

	UButton* CreateStyledButton(const FString& Label, const FLinearColor& BaseColor, const FLinearColor& HoverColor);
	void StyleButton(UButton* Button, const FLinearColor& BaseColor, const FLinearColor& HoverColor);

	UFUNCTION()
	void HandleRespond();

	UFUNCTION()
	void HandleIgnore();

	UFUNCTION()
	void HandleExit();
};

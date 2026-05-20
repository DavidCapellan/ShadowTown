// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShadowPauseMenuWidget.generated.h"

class UButton;

/**
 * Custom pause menu widget class that handles resume, restart, and quit actions.
 */
UCLASS()
class SHADOWTOWN_API UShadowPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BtnResume;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BtnRestart;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BtnQuit;

private:
	UFUNCTION()
	void HandleResume();

	UFUNCTION()
	void HandleRestart();

	UFUNCTION()
	void HandleQuit();
};

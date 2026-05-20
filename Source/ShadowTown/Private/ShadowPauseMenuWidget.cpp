// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowPauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UShadowPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BtnResume)
	{
		BtnResume->OnClicked.AddDynamic(this, &UShadowPauseMenuWidget::HandleResume);
	}

	if (BtnRestart)
	{
		BtnRestart->OnClicked.AddDynamic(this, &UShadowPauseMenuWidget::HandleRestart);
	}

	if (BtnQuit)
	{
		BtnQuit->OnClicked.AddDynamic(this, &UShadowPauseMenuWidget::HandleQuit);
	}
}

void UShadowPauseMenuWidget::HandleResume()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	RemoveFromParent();
}

void UShadowPauseMenuWidget::HandleRestart()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
}

void UShadowPauseMenuWidget::HandleQuit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, true);
}

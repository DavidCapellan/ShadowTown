#include "ShadowInteractionMenuWidget.h"
#include "ShadowPlayerInteractionLibrary.h"
#include "ShadowDialogueSubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/Character.h"

namespace
{
	UCanvasPanelSlot* AddToCanvas(UCanvasPanel* Canvas, UWidget* Child, const FAnchors& Anchors, const FVector2D& Alignment, const FVector2D& Position, const FVector2D& Size)
	{
		if (!Canvas || !Child)
		{
			return nullptr;
		}

		Canvas->AddChild(Child);
		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Child->Slot))
		{
			Slot->SetAnchors(Anchors);
			Slot->SetAlignment(Alignment);
			Slot->SetPosition(Position);
			Slot->SetSize(Size);
			Slot->SetAutoSize(false);
		}
		return Cast<UCanvasPanelSlot>(Child->Slot);
	}
}

void UShadowInteractionMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (bUiBuilt)
	{
		return;
	}

	bUiBuilt = true;

	// Si los botones ya existen (BindWidget desde Blueprint), solo los configuramos
	if (BtnResponder || BtnIgnorar || BtnSalir)
	{
		if (BtnResponder)
		{
			BtnResponder->OnClicked.AddDynamic(this, &UShadowInteractionMenuWidget::HandleRespond);
		}
		if (BtnIgnorar)
		{
			BtnIgnorar->OnClicked.AddDynamic(this, &UShadowInteractionMenuWidget::HandleIgnore);
		}
		if (BtnSalir)
		{
			BtnSalir->OnClicked.AddDynamic(this, &UShadowInteractionMenuWidget::HandleExit);
		}
		return;
	}

	if (!WidgetTree)
	{
		return;
	}

	// Fallback: Construir UI por código si el Blueprint está vacío
	RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	UOverlay* ScreenOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("ScreenOverlay"));
	RootCanvas->AddChild(ScreenOverlay);
	if (UCanvasPanelSlot* OverlaySlot = Cast<UCanvasPanelSlot>(ScreenOverlay->Slot))
	{
		OverlaySlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		OverlaySlot->SetOffsets(FMargin(0.f));
	}

	UBorder* DimBackground = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DimBackground"));
	DimBackground->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.45f));
	ScreenOverlay->AddChild(DimBackground);

	UCanvasPanel* MenuCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("MenuCanvas"));
	ScreenOverlay->AddChild(MenuCanvas);
	if (UOverlaySlot* MenuOverlaySlot = Cast<UOverlaySlot>(MenuCanvas->Slot))
	{
		MenuOverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		MenuOverlaySlot->SetVerticalAlignment(VAlign_Fill);
	}

	const FVector2D BtnSize(240.f, 60.f);
	const float DiamondSpreadX = 160.f;
	const float DiamondSpreadY = 75.f;
	const float BaseY = -150.f;

	BtnResponder = CreateStyledButton(TEXT("RESPONDER"), FLinearColor(0.05f, 0.40f, 0.15f, 0.85f), FLinearColor(0.10f, 0.75f, 0.25f, 1.f));
	BtnIgnorar = CreateStyledButton(TEXT("IGNORAR"), FLinearColor(0.50f, 0.08f, 0.08f, 0.85f), FLinearColor(0.90f, 0.15f, 0.15f, 1.f));
	BtnSalir = CreateStyledButton(TEXT("SALIR"), FLinearColor(0.05f, 0.05f, 0.07f, 0.85f), FLinearColor(0.25f, 0.25f, 0.30f, 1.f));

	AddToCanvas(MenuCanvas, BtnResponder, FAnchors(0.5f, 1.f, 0.5f, 1.f), FVector2D(0.5f, 1.f), FVector2D(0.f, BaseY - DiamondSpreadY), BtnSize);
	AddToCanvas(MenuCanvas, BtnIgnorar, FAnchors(0.5f, 1.f, 0.5f, 1.f), FVector2D(0.5f, 1.f), FVector2D(-DiamondSpreadX, BaseY + (DiamondSpreadY * 0.5f)), BtnSize);
	AddToCanvas(MenuCanvas, BtnSalir, FAnchors(0.5f, 1.f, 0.5f, 1.f), FVector2D(0.5f, 1.f), FVector2D(DiamondSpreadX, BaseY + (DiamondSpreadY * 0.5f)), BtnSize);

	if (BtnResponder)
	{
		BtnResponder->OnClicked.AddDynamic(this, &UShadowInteractionMenuWidget::HandleRespond);
	}
	if (BtnIgnorar)
	{
		BtnIgnorar->OnClicked.AddDynamic(this, &UShadowInteractionMenuWidget::HandleIgnore);
	}
	if (BtnSalir)
	{
		BtnSalir->OnClicked.AddDynamic(this, &UShadowInteractionMenuWidget::HandleExit);
	}
}

TSharedRef<SWidget> UShadowInteractionMenuWidget::RebuildWidget()
{
	NativeConstruct();
	return Super::RebuildWidget();
}

UButton* UShadowInteractionMenuWidget::CreateStyledButton(
	const FString& Label,
	const FLinearColor& BaseColor,
	const FLinearColor& HoverColor)
{
	if (!WidgetTree)
	{
		return nullptr;
	}

	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	StyleButton(Button, BaseColor, HoverColor);

	UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Text->SetText(FText::FromString(Label));
	Text->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.95f, 0.95f, 1.f)));
	FSlateFontInfo Font = Text->GetFont();
	Font.Size = 18;
	Font.TypefaceFontName = TEXT("Bold");
	Font.LetterSpacing = 150;
	Text->SetFont(Font);
	Text->SetJustification(ETextJustify::Center);
	Text->SetShadowOffset(FVector2D(1.f, 1.f));
	Text->SetShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.5f));

	Button->AddChild(Text);
	return Button;
}

void UShadowInteractionMenuWidget::StyleButton(UButton* Button, const FLinearColor& BaseColor, const FLinearColor& HoverColor)
{
	if (!Button)
	{
		return;
	}

	auto MakeBrush = [](const FLinearColor& Color, bool bHovered = false) -> FSlateBrush
	{
		FSlateBrush Brush;
		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		Brush.TintColor = FSlateColor(Color);
		Brush.OutlineSettings.CornerRadii = FVector4(12.f, 12.f, 12.f, 12.f);
		Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		if (bHovered)
		{
			Brush.OutlineSettings.Width = 3.0f;
			Brush.OutlineSettings.Color = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.8f));
		}
		else
		{
			Brush.OutlineSettings.Width = 1.5f;
			Brush.OutlineSettings.Color = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.3f));
		}
		return Brush;
	};

	FButtonStyle Style;
	Style.Normal = MakeBrush(BaseColor, false);
	Style.Hovered = MakeBrush(HoverColor, true);
	Style.Pressed = MakeBrush(BaseColor * 0.75f, true);
	Style.Disabled = MakeBrush(BaseColor * 0.4f, false);
	Style.NormalPadding = FMargin(24.f, 12.f);
	Style.PressedPadding = FMargin(24.f, 12.f);
	Button->SetStyle(Style);
}

void UShadowInteractionMenuWidget::ShowMenu(ACharacter* PlayerCharacter, AActor* NpcActor)
{
	CachedPlayer = PlayerCharacter;
	CachedNpc = NpcActor;
	SetVisibility(ESlateVisibility::Visible);

	if (CachedPlayer.IsValid())
	{
		UShadowPlayerInteractionLibrary::LockPlayerMovementForMenu(CachedPlayer.Get());
	}
}

void UShadowInteractionMenuWidget::HideMenu()
{
	SetVisibility(ESlateVisibility::Collapsed);
	// aqui falla hay que mirarlo
	if (CachedPlayer.IsValid())
	{
		UShadowPlayerInteractionLibrary::UnlockPlayerMovementForMenu(CachedPlayer.Get());
	}

	CachedPlayer.Reset();
	CachedNpc.Reset();
}

void UShadowInteractionMenuWidget::HandleRespond()
{
	OnRespondClicked();
	OnRespond.Broadcast();
}

void UShadowInteractionMenuWidget::HandleIgnore()
{
	OnIgnoreClicked();
	OnIgnore.Broadcast();
}

void UShadowInteractionMenuWidget::HandleExit()
{
	OnExitClicked();
	OnExit.Broadcast();
	UShadowPlayerInteractionLibrary::HideInteractionMenu(CachedPlayer.Get());
}

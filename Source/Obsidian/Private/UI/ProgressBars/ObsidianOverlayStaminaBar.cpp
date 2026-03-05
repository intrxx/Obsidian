// Copyright 2026 out of sCope - intrxx

#include "UI/ProgressBars/ObsidianOverlayStaminaBar.h"

#include <Kismet/KismetMathLibrary.h>
#include <Components/ProgressBar.h>

#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"

void UObsidianOverlayStaminaBar::HandleWidgetControllerSet()
{
	MainOverlayWidgetController = Cast<UObMainOverlayWidgetController>(WidgetController);
	check(MainOverlayWidgetController);

	MainOverlayWidgetController->OnStaminaChangedDelegate.AddDynamic(this, &ThisClass::StaminaChanged);
	MainOverlayWidgetController->OnMaxStaminaChangedDelegate.AddDynamic(this, &ThisClass::MaxStaminaChanged);

	MainOverlayWidgetController->SetInitialStaminaValues();
}

FReply UObsidianOverlayStaminaBar::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

FReply UObsidianOverlayStaminaBar::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

void UObsidianOverlayStaminaBar::StaminaChanged(const float NewValue)
{
	Stamina = NewValue;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(Stamina, MaxStamina);
	
	if (Stamina_ProgressBar)
	{
		Stamina_ProgressBar->SetPercent(ProgressBarPercent);
	}
}

void UObsidianOverlayStaminaBar::MaxStaminaChanged(const float NewValue)
{
	MaxStamina = NewValue;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(Stamina, MaxStamina);

	if (Stamina_ProgressBar)
	{
		Stamina_ProgressBar->SetPercent(ProgressBarPercent);
	}
}

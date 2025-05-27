// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ProgressBars/ObsidianHeroStaggerBar.h"

// ~ Core

// ~ Project
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

void UObsidianHeroStaggerBar::HandleWidgetControllerSet()
{
	UMainOverlayWidgetController* WC = Cast<UMainOverlayWidgetController>(WidgetController);
	if(WC == nullptr)
	{
		return;
	}

	WC->OnStaggerMeterChangedDelegate.AddDynamic(this, &ThisClass::StaggerChanged);
	WC->OnMaxStaggerMeterChangedDelegate.AddDynamic(this, &ThisClass::MaxStaggerChanged);

	WC->SetInitialStaggerMeter();
}

void UObsidianHeroStaggerBar::StaggerChanged(const float NewStagger)
{
	Stagger = NewStagger;
	SetProgressBarPercent(Stagger, MaxStagger, Stagger_ProgressBar);
}

void UObsidianHeroStaggerBar::MaxStaggerChanged(const float NewMaxStagger)
{
	MaxStagger = NewMaxStagger;
	SetProgressBarPercent(Stagger, MaxStagger, Stagger_ProgressBar);
}

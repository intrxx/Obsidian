// Copyright 2026 out of sCope team - intrxx

#include "UI/ProgressBars/ObsidianHeroStaggerBar.h"

#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"


void UObsidianHeroStaggerBar::HandleWidgetControllerSet()
{
	UObMainOverlayWidgetController* WC = Cast<UObMainOverlayWidgetController>(WidgetController);
	if(WC == nullptr)
	{
		return;
	}

	WC->OnStaggerMeterChangedDelegate.AddDynamic(this, &ThisClass::StaggerChanged);
	WC->OnMaxStaggerMeterChangedDelegate.AddDynamic(this, &ThisClass::MaxStaggerChanged);

	WC->SetInitialStaggerMeter();
}

void UObsidianHeroStaggerBar::StaggerChanged(const float InNewStagger)
{
	Stagger = InNewStagger;
	SetProgressBarPercent(Stagger, MaxStagger, Stagger_ProgressBar);
}

void UObsidianHeroStaggerBar::MaxStaggerChanged(const float InNewMaxStagger)
{
	MaxStagger = InNewMaxStagger;
	SetProgressBarPercent(Stagger, MaxStagger, Stagger_ProgressBar);
}

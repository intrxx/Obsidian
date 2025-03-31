// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianOverlayExperienceBar.h"
#include "Components/ProgressBar.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

void UObsidianOverlayExperienceBar::HandleWidgetControllerSet()
{
	UMainOverlayWidgetController* MainOverlayWidgetController = Cast<UMainOverlayWidgetController>(WidgetController);
	check(MainOverlayWidgetController);

	MainOverlayWidgetController->OnExperienceChangedDelegate.BindUObject(this, &ThisClass::ExperienceChanged);
	MainOverlayWidgetController->OnMaxExperienceChangedDelegate.BindUObject(this, &ThisClass::MaxExperienceChanged);

	MainOverlayWidgetController->SetInitialExperienceValues();
}

void UObsidianOverlayExperienceBar::ExperienceChanged(const float NewValue)
{
	Experience = NewValue;

	float BarPercentage = 0.0f;
	if(MaxExperience > 0.0f)
	{
		BarPercentage = (Experience - LastMaxExperience) / (MaxExperience - LastMaxExperience);
	}

	if(Experience_ProgressBar)
	{
		Experience_ProgressBar->SetPercent(BarPercentage);
	}
}

void UObsidianOverlayExperienceBar::MaxExperienceChanged(const float NewValue, const float OldValue)
{
	MaxExperience = NewValue;
	LastMaxExperience = OldValue;
	
	float BarPercentage = 0.0f;
	if(MaxExperience > 0.0f)
	{
		BarPercentage = (Experience - LastMaxExperience) / (MaxExperience - LastMaxExperience);
	}

	if(Experience_ProgressBar)
	{
		Experience_ProgressBar->SetPercent(BarPercentage);
	}
}

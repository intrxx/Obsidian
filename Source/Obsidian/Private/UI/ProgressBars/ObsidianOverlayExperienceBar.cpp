// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianOverlayExperienceBar.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/ProgressBar.h"
#include "UI/MainOverlay/Subwidgets/ObsidianOverlayExperienceInfo.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

void UObsidianOverlayExperienceBar::HandleWidgetControllerSet()
{
	MainOverlayWidgetController = Cast<UMainOverlayWidgetController>(WidgetController);
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

void UObsidianOverlayExperienceBar::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(ExperienceInfo == nullptr)
	{
		if(MainOverlayWidgetController == nullptr)
		{
			return;
		}

		APlayerController* OwningPC = MainOverlayWidgetController->GetOwningPlayerController();
		if(OwningPC == nullptr)
		{
			return;
		}
	
		checkf(ExperienceInfoWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianOverlayExperienceBar::NativeOnMouseEnter, fill it in UObsidianOverlayExperienceBar on Main Overlay."));
		ExperienceInfo = CreateWidget<UObsidianOverlayExperienceInfo>(OwningPC, ExperienceInfoWidgetClass);
		ExperienceInfo->InitializeExperienceInfo(Experience, MaxExperience, LastMaxExperience);
	
		const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
		ExperienceInfo->SetPositionInViewport(MousePosition + FVector2d(0.0f, -100.0f));
		ExperienceInfo->AddToViewport();
	}
}

void UObsidianOverlayExperienceBar::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(ExperienceInfo)
	{
		ExperienceInfo->DestroyExperienceInfo();
		ExperienceInfo = nullptr;
	}
}

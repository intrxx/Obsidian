// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianOverlayExperienceBar.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
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
		if(IsValid(MainOverlayWidgetController) == false)
		{
			return;
		}

		AObsidianPlayerController* OwningPC = MainOverlayWidgetController->GetOwningPlayerController();
		if(IsValid(OwningPC) == false)
		{
			return;
		}

		AObsidianPlayerState* OwningPS = MainOverlayWidgetController->GetOwningPlayerState();
		if(IsValid(OwningPS) == false)
		{
			return;
		}
		
		UWorld* World = GetWorld();
		if(World == nullptr)
		{
			return;
		}
		
		checkf(ExperienceInfoWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianOverlayExperienceBar::NativeOnMouseEnter, fill it in UObsidianOverlayExperienceBar on Main Overlay."));
		ExperienceInfo = CreateWidget<UObsidianOverlayExperienceInfo>(OwningPC, ExperienceInfoWidgetClass);
		ExperienceInfo->InitializeExperienceInfo(Experience, MaxExperience, LastMaxExperience, OwningPS->GetHeroLevel());
		
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(World);
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
		const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
		MousePosition *= DPIScale;
		
		const FVector2D InfoPosition = FVector2D(MousePosition.X, ViewportSize.Y) + (ExperienceInfo->ScreenDisplayOffset * DPIScale);
		
		ExperienceInfo->SetPositionInViewport(InfoPosition);
		ExperienceInfo->SetAlignmentInViewport(FVector2D(0.0f, 1.0f));
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

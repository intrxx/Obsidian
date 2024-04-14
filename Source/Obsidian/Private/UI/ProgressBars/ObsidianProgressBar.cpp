// Copyright 2024 Michał Ogiński

#include "Components/ProgressBar.h"
#include "UI/ProgressBars/ObsidianProgressGlobe.h"

void UObsidianProgressGlobe::SetInfoGlobeVisibility(bool bShouldBeVisible)
{
	if(InfoProgressGlobe == nullptr)
	{
		return;
	}

	if(bShouldBeVisible)
	{
		InfoProgressGlobe->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InfoProgressGlobe->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UObsidianProgressGlobe::SetGhostGlobeDecreasing(float CurrentPercent, float NewPercent, float DeltaTime)
{
	CurrentPercentage = FMath::FInterpTo(CurrentPercent, NewPercent, DeltaTime, GhostGlobeFollowingSpeed);
	GhostProgressGlobe->SetPercent(CurrentPercentage);
	
	if(CurrentPercent <= NewPercent)
	{
		bShouldSetGhostGlobe = false;
	}
}

void UObsidianProgressGlobe::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if(bShouldSetGhostGlobe)
	{
		SetGhostGlobeDecreasing(CurrentPercentage, NewPercentage, InDeltaTime);
	}
}

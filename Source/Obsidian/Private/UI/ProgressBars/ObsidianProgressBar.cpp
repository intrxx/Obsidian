// Copyright 2024 Michał Ogiński

#include "Components/ProgressBar.h"
#include "UI/ProgressBars/ObsidianProgressGlobe.h"

void UObsidianProgressGlobe::SetProgressGlobePercent(float Percent)
{
	ProgressGlobe->SetPercent(Percent);
}

void UObsidianProgressGlobe::SetGhostGlobeDecreasing(float CurrentPercent, float NewPercent, float DeltaTime)
{
	CurrentPercentage = FMath::FInterpTo(CurrentPercent, NewPercent, DeltaTime, 5.f);
	//UE_LOG(LogTemp, Warning, TEXT("Result of interp: %f"), CurrentPercent);
	GhostProgressGlobe->SetPercent(CurrentPercentage);

	if(CurrentPercent <= NewPercent)
	{
		bShouldSetGhostGlobe = false;
	}
}

void UObsidianProgressGlobe::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(bShouldSetGhostGlobe && GhostProgressGlobe)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Decresing, CurrentPerc: %f NewPerc: %f"), CurrentPercentage, NewPercentage);
		SetGhostGlobeDecreasing(CurrentPercentage, NewPercentage, InDeltaTime);
	}
}

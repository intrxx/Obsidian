// Copyright 2024 Michał Ogiński

#include "Components/ProgressBar.h"
#include "UI/ProgressBars/ObsidianProgressGlobe.h"

void UObsidianProgressGlobe::SetProgressGlobePercent(float Percent)
{
	ProgressGlobe->SetPercent(Percent);
}

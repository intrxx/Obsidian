// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianBasicHealthBar.h"

#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

void UObsidianBasicHealthBar::SetProgressBarPercent(const float Value, const float MaxValue, UProgressBar* ProgressBar)
{
    if(ProgressBar)
    {
        ProgressBar->SetPercent(UKismetMathLibrary::SafeDivide(Value, MaxValue));
    }
}
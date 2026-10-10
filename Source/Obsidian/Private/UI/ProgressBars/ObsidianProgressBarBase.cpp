// Copyright 2026 out of sCope team - intrxx

#include "UI/ProgressBars/ObsidianProgressBarBase.h"

#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"


bool UObsidianProgressBarBase::GetEffectFillImageForTag(FObsidianProgressBarEffectFillImage& OutFillImage, FGameplayTag InEffectTag)
{
	for(const FObsidianProgressBarEffectFillImage& EffectFillImage : ProgressBarEffectFillImages)
	{
		if(EffectFillImage.ProgressBarFillImage.IsSet() && (EffectFillImage.EffectTag == InEffectTag))
		{
			OutFillImage = EffectFillImage;
			return true;
		}
	}
	return false;
}

void UObsidianProgressBarBase::SetProgressBarPercent(const float InValue, const float InMaxValue, UProgressBar* InProgressBar)
{
	if(InProgressBar)
	{
		InProgressBar->SetPercent(UKismetMathLibrary::SafeDivide(InValue, InMaxValue));
	}
}


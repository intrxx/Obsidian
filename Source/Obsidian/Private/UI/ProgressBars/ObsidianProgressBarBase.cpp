// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianProgressBarBase.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

bool UObsidianProgressBarBase::GetEffectFillImageForTag(FObsidianProgressBarEffectFillImage& OutFillImage, const FGameplayTag& EffectTag)
{
	for(const FObsidianProgressBarEffectFillImage& EffectFillImage : ProgressBarEffectFillImages)
	{
		if(EffectFillImage.ProgressBarFillImage.IsSet() && (EffectFillImage.EffectTag == EffectTag))
		{
			OutFillImage = EffectFillImage;
			return true;
		}
	}
	return false;
}

void UObsidianProgressBarBase::SetProgressBarPercent(const float Value, const float MaxValue, UProgressBar* ProgressBar)
{
	if(ProgressBar)
	{
		ProgressBar->SetPercent(UKismetMathLibrary::SafeDivide(Value, MaxValue));
	}
}


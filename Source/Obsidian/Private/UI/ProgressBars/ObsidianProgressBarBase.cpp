// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianProgressBarBase.h"

bool UObsidianProgressBarBase::GetEffectFillImageForTag(FSlateBrush& OutFillImage, const FGameplayTag& EffectTag)
{
	for(const FObsidianProgressBarEffectFillImage& EffectFillImage : ProgressBarEffectFillImages)
	{
		if(EffectFillImage.ProgressBarFillImage.IsSet() && (EffectFillImage.EffectTag == EffectTag))
		{
			OutFillImage = EffectFillImage.ProgressBarFillImage;
			return true;
		}
	}
	return false;
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianProgressBarBase.h"

#include "Obsidian/Obsidian.h"

FSlateBrush UObsidianProgressBarBase::GetEffectFillImageForTag(const FGameplayTag& EffectTag, bool bLogNotFound)
{
	for(const FObsidianProgressBarEffectFillImage& EffectFillImage : ProgressBarEffectFillImages)
	{
		if(EffectFillImage.ProgressBarFillImage.IsSet() && (EffectFillImage.EffectTag == EffectTag))
		{
			return EffectFillImage.ProgressBarFillImage;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogObsidian, Display, TEXT("Could not find ProgressBarFillImage for [%s] on ObsidianProgressBarBase [%s]"),
			*EffectTag.ToString(), *GetNameSafe(this));
	}

	return FSlateBrush();
}

// Copyright 2024 Michał Ogiński


#include "UI/Subwidgets/ObsidianDurationalEffectInfo.h"
#include "CommonTextBlock.h"

void UObsidianDurationalEffectInfo::StartEffectTimer()
{
	if(EffectTimer_TextBlock)
	{
		const int32 Minutes = FMath::FloorToInt(EffectDuration / 60.f);
		const int32 Seconds = EffectDuration - Minutes * 60.f;
		
		const FText Time = FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
		
		if(Minutes == 0 && Seconds <= 3)
		{
			EffectTimer_TextBlock->SetStyle(ExpiringTimerStyle);
		}
		else
		{
			EffectTimer_TextBlock->SetStyle(NormalTimerStyle);
		}
		
		EffectTimer_TextBlock->SetText(Time);
	}
}
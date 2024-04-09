// Copyright 2024 Michał Ogiński


#include "UI/Subwidgets/ObsidianDurationalEffectInfo.h"
#include "CommonTextBlock.h"

void UObsidianDurationalEffectInfo::StartEffectTimer()
{
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &ThisClass::TimerCountDown,
		1.f, true, 0.f);
}

void UObsidianDurationalEffectInfo::TimerCountDown()
{
	if(EffectTimer_TextBlock)
	{
		EffectDuration--;
		
		const int32 Minutes = FMath::FloorToInt(EffectDuration / 60.f);
		const int32 Seconds = EffectDuration - Minutes * 60.f;
		
		const FText Time = FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
		
		EffectTimer_TextBlock->SetText(Time);
		
		if(EffectDuration < 0)
		{
			if(EffectDurationTimerHandle.IsValid())
			{
				GetOwningPlayer()->GetWorldTimerManager().ClearTimer(EffectDurationTimerHandle);
			}
			OnEffectUnHovered();
			RemoveFromParent();
		}
	}
}

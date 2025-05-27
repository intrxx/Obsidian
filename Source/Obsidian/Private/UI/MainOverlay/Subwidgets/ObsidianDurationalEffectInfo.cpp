// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/MainOverlay/Subwidgets/ObsidianDurationalEffectInfo.h"

// ~ Core
#include "CommonTextBlock.h"

// ~ Project

void UObsidianDurationalEffectInfo::StartEffectTimer()
{
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayer() : OwningPlayer;
	OwningPlayer->GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &ThisClass::TimerCountDown,
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
				OwningPlayer->GetWorldTimerManager().ClearTimer(EffectDurationTimerHandle);
			}
			OnEffectUnHovered();
			RemoveFromParent();

			OnDurationalInfoWidgetTerminatedDelegate.Broadcast(this);
		}
	}
}

// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/MainOverlay/Subwidgets/OStackingDurationalEffectInfo.h"

// ~ Core
#include "CommonTextBlock.h"

// ~ Project
#include "Obsidian/ObsidianGameModule.h"

void UOStackingDurationalEffectInfo::SetStackCount(const int32 Count)
{
	EffectStackCount = Count;
	if(StackCount_TextBlock)
	{
		StackCount_TextBlock->SetText(FText::AsNumber(Count));
	}
}

void UOStackingDurationalEffectInfo::UpdateStackingInfoWidget(const int32 NewCount)
{
	SetStackCount(NewCount);
	if(EffectDurationPolicy == EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication)
	{
		ResetEffectTimer();
	}
	else if(EffectDurationPolicy == EGameplayEffectStackingDurationPolicy::NeverRefresh)
	{
		UE_LOG(LogObsidian, Error, TEXT("EGameplayEffectStackingDurationPolicy::NeverRefresh is not implemented on [%s]"), *GetNameSafe(this));
	}
}

void UOStackingDurationalEffectInfo::StartEffectTimer()
{
	WholeEffectDuration = EffectDuration;
	
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayer() : OwningPlayer;
	OwningPlayer->GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &ThisClass::TimerCountDown,
		1.f, true, 0.f);
}

void UOStackingDurationalEffectInfo::ResetEffectTimer()
{
	EffectDuration = WholeEffectDuration;
	
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayer() : OwningPlayer;
	if(EffectDurationTimerHandle.IsValid())
	{
		OwningPlayer->GetWorldTimerManager().ClearTimer(EffectDurationTimerHandle);
	}

	OwningPlayer->GetWorldTimerManager().SetTimer(EffectDurationTimerHandle, this, &ThisClass::TimerCountDown,
		1.f, true, 0.f);
}

void UOStackingDurationalEffectInfo::TimerCountDown()
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
			HandleEffectExpiration();
		}
	}
}

void UOStackingDurationalEffectInfo::HandleEffectExpiration()
{
	switch(EffectExpirationPolicy)
	{
	case EGameplayEffectStackingExpirationPolicy::ClearEntireStack:
		TerminateTheEffect();
		break;
	case EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration:
		if(EffectStackCount-1 == 0)
		{
			TerminateTheEffect();
			return;
		}
		UpdateStackingInfoWidget(EffectStackCount-1);
		break;
	case EGameplayEffectStackingExpirationPolicy::RefreshDuration:
		UE_LOG(LogObsidian, Error, TEXT("EGameplayEffectStackingExpirationPolicy::RefreshDuration is not implemented on [%s]"), *GetNameSafe(this));
		break;
	default:
		break;
	}
}

void UOStackingDurationalEffectInfo::TerminateTheEffect()
{
	OnEffectUnHovered();
	RemoveFromParent();

	OnStackingInfoWidgetTerminatedDelegate.Broadcast(this);
	
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayer() : OwningPlayer;
	if(EffectDurationTimerHandle.IsValid())
	{
		OwningPlayer->GetWorldTimerManager().ClearTimer(EffectDurationTimerHandle);
	}
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/MainOverlay/Subwidgets/ObsidianTimerWidget.h"
#include "CommonTextBlock.h"

void UObsidianTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(SyncTimerHandle, this, &ThisClass::UpdateTimer, SyncClockFrequency,
			true, 0.0f);	
	}
}

void UObsidianTimerWidget::NativeDestruct()
{
	if(SyncTimerHandle.IsValid())
	{
		SyncTimerHandle.Invalidate();
	}
	
	Super::NativeDestruct();
}

void UObsidianTimerWidget::UpdateTimer()
{
	const int32 HourNow = FDateTime::Now().GetHour();
	const int32 MinuteNow = FDateTime::Now().GetMinute();
	const FText TimeNowText = FText::FromString(FString::Printf(TEXT("%d:%d"), HourNow, MinuteNow));

	Timer_Text->SetText(TimeNowText);
}

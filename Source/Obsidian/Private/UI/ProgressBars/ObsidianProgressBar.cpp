// Copyright 2024 out of sCope team - Michał Ogiński


#include "CommonTextBlock.h"
#include "Components/ProgressBar.h"
#include "UI/ProgressBars/ObsidianProgressGlobe.h"

void UObsidianProgressGlobe::SetInfoGlobeVisibility(const bool bShouldBeVisible)
{
	if(InfoProgressGlobe == nullptr)
	{
		return;
	}

	if(bShouldBeVisible)
	{
		InfoProgressGlobe->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InfoProgressGlobe->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UObsidianProgressGlobe::SetSecondAttributeName(const FText SecondAttributeNameToSet)
{
	if(SecondAttributeNameText)
	{
		SecondAttributeNameText->SetText(SecondAttributeNameToSet);
	}
}

void UObsidianProgressGlobe::SetProgressGlobeStyle(const FSlateBrush ProgressGlobeFillImage)
{
	if(ProgressGlobe)
	{
		FProgressBarStyle Style;
		Style.BackgroundImage.TintColor = FSlateColor(FLinearColor::Transparent);
		Style.FillImage = ProgressGlobeFillImage;
		ProgressGlobe->SetWidgetStyle(Style);
	}
}

void UObsidianProgressGlobe::ResetStyle()
{
	if(ProgressGlobe)
	{
		FProgressBarStyle Style;
		Style.BackgroundImage.TintColor = FSlateColor(FLinearColor::Transparent);
		Style.FillImage = GlobeFillImage;
		ProgressGlobe->SetWidgetStyle(Style);
	}
}

void UObsidianProgressGlobe::SetGhostGlobeDecreasing(float CurrentPercent, float NewPercent, float DeltaTime)
{
	CurrentPercentage = FMath::FInterpTo(CurrentPercent, NewPercent, DeltaTime, GhostGlobeFollowingSpeed);
	GhostProgressGlobe->SetPercent(CurrentPercentage);
	
	if(CurrentPercent <= NewPercent)
	{
		bShouldSetGhostGlobe = false;
	}
}

void UObsidianProgressGlobe::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if(bShouldSetGhostGlobe)
	{
		SetGhostGlobeDecreasing(CurrentPercentage, NewPercentage, InDeltaTime);
	}
}

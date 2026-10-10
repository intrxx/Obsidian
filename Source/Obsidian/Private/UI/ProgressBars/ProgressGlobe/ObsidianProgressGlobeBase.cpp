// Copyright 2026 out of sCope team - intrxx

#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetMathLibrary.h"

#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"


void UObsidianProgressGlobe::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	SetInfoGlobeVisibility(false);
}

void UObsidianProgressGlobe::ShouldGhostGlobeDecrease(const float InNewAttribute, const float InAttribute, const float InMaxAttribute)
{
	if(InNewAttribute == InAttribute)
	{
		return;
	}

	CurrentPercentage = UKismetMathLibrary::SafeDivide(InAttribute, InMaxAttribute);
	NewPercentage = UKismetMathLibrary::SafeDivide(InNewAttribute, InMaxAttribute);

	if(InAttribute > InNewAttribute)
	{
		bShouldSetGhostGlobe = true;
		return;
	}
	bShouldSetGhostGlobe = false;
}

FReply UObsidianProgressGlobe::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

FReply UObsidianProgressGlobe::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

void UObsidianProgressGlobe::HandleWidgetControllerSet()
{
	MainOverlayWidgetController = Cast<UObMainOverlayWidgetController>(WidgetController);
	check(WidgetController);
}

void UObsidianProgressGlobe::SetInfoGlobeVisibility(const bool bInShouldBeVisible)
{
	if(Info_ProgressGlobe == nullptr)
	{
		return;
	}

	if(bInShouldBeVisible)
	{
		Info_ProgressGlobe->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Info_ProgressGlobe->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UObsidianProgressGlobe::SetGhostGlobeDecreasing(const float InCurrentPercent, const float InNewPercent, const float InDeltaTime)
{
	CurrentPercentage = FMath::FInterpTo(InCurrentPercent, InNewPercent, InDeltaTime, GhostGlobeFollowingSpeed);
	Ghost_ProgressGlobe->SetPercent(CurrentPercentage);
	
	if(InCurrentPercent <= InNewPercent)
	{
		bShouldSetGhostGlobe = false;
	}
}

void UObsidianProgressGlobe::NativeTick(const FGeometry& InMyGeometry, float InDeltaTime)
{
	Super::NativeTick(InMyGeometry, InDeltaTime);
	
	if(bShouldSetGhostGlobe)
	{
		SetGhostGlobeDecreasing(CurrentPercentage, NewPercentage, InDeltaTime);
	}
}

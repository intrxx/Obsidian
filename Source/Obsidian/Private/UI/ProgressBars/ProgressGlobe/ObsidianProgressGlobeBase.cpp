// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h"

// ~ Core
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetMathLibrary.h"

// ~ Project
#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"

void UObsidianProgressGlobe::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	SetInfoGlobeVisibility(false);
}

void UObsidianProgressGlobe::ShouldGhostGlobeDecrease(const float NewAttribute, const float Attribute, const float MaxAttribute)
{
	if(NewAttribute == Attribute)
	{
		return;
	}

	CurrentPercentage = UKismetMathLibrary::SafeDivide(Attribute, MaxAttribute);
	NewPercentage = UKismetMathLibrary::SafeDivide(NewAttribute, MaxAttribute);

	if(Attribute > NewAttribute)
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

void UObsidianProgressGlobe::SetInfoGlobeVisibility(const bool bShouldBeVisible)
{
	if(Info_ProgressGlobe == nullptr)
	{
		return;
	}

	if(bShouldBeVisible)
	{
		Info_ProgressGlobe->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Info_ProgressGlobe->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UObsidianProgressGlobe::SetGhostGlobeDecreasing(const float CurrentPercent, const float NewPercent, const float DeltaTime)
{
	CurrentPercentage = FMath::FInterpTo(CurrentPercent, NewPercent, DeltaTime, GhostGlobeFollowingSpeed);
	Ghost_ProgressGlobe->SetPercent(CurrentPercentage);
	
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

// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

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

void UObsidianProgressGlobe::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	OnMouseEnterLeaveDelegate.Broadcast(true);
}

void UObsidianProgressGlobe::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	OnMouseEnterLeaveDelegate.Broadcast(false);
}

FReply UObsidianProgressGlobe::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// @HACK displaying mouse button down event fixes a bug when we click on the globe, move the mouse over and have our mouse movement blocked.
	return FReply::Handled();
}

void UObsidianProgressGlobe::HandleWidgetControllerSet()
{
	MainOverlayWidgetController = Cast<UMainOverlayWidgetController>(WidgetController);
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

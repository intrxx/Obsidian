// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h"
#include "CommonTextBlock.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"


void UObsidianProgressGlobe::NativePreConstruct()
{
	Super::NativePreConstruct();

	RootSizeBox->SetWidthOverride(RootBoxWidth);
	RootSizeBox->SetHeightOverride(RootBoxHeight);
	GlobeWrapperImage->SetBrush(GlobeWrapperBrush);

	//TODO Set the alpha back to 1 when using wrapper image
	FSlateBrush GlobeStyle;
	GlobeStyle.TintColor = FSlateColor(FLinearColor::Transparent);
	
	FProgressBarStyle BarStyle;
	BarStyle.FillImage = GlobeFillImage;
	BarStyle.BackgroundImage = GlobeStyle;
	
	ProgressGlobe->SetWidgetStyle(BarStyle);

	UWidgetLayoutLibrary::SlotAsOverlaySlot(ProgressGlobe)->SetPadding(GlobeMargin);

	FirstAttributeNameText->SetText(FirstAttributeName);
	SecondAttributeNameText->SetText(SecondAttributeName);
	
	FProgressBarStyle GhostGlobeBarStyle;
	GhostGlobeBarStyle.FillImage = GhostGlobeFillImage;
	GhostGlobeBarStyle.BackgroundImage = GlobeStyle;
	
	GhostProgressGlobe->SetWidgetStyle(GhostGlobeBarStyle);
	
	FProgressBarStyle InfoGlobeBarStyle;
	InfoGlobeBarStyle.FillImage = InfoGlobeFillImage;
	InfoGlobeBarStyle.BackgroundImage = GlobeStyle;
	
	InfoProgressGlobe->SetWidgetStyle(InfoGlobeBarStyle);
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

void UObsidianProgressGlobe::HandleWidgetControllerSet()
{
	MainOverlayWidgetController = Cast<UMainOverlayWidgetController>(WidgetController);
	check(WidgetController);
}

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

void UObsidianProgressGlobe::SetSecondAttributeName(const FText& SecondAttributeNameToSet)
{
	if(SecondAttributeNameText)
	{
		SecondAttributeNameText->SetText(SecondAttributeNameToSet);
	}
}

void UObsidianProgressGlobe::SetProgressGlobeStyle(const FSlateBrush& ProgressGlobeFillImage)
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

void UObsidianProgressGlobe::SetGhostGlobeDecreasing(const float CurrentPercent, const float NewPercent, const float DeltaTime)
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

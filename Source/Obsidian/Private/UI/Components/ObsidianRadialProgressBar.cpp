// Copyright 2026 out of sCope team - intrxx

#include "UI/Components/ObsidianRadialProgressBar.h"

// ~ Core

// ~ Project
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UObsidianRadialProgressBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if(IsDesignTime())
	{
		SetPercent(Percent);
	}
	SetThickness(Thickness);

	Root_SizeBox->SetWidthOverride(CircularWidth);
	Root_SizeBox->SetHeightOverride(CircularHeight);
}

bool UObsidianRadialProgressBar::CreateProgressBarMaterial()
{
	if(ProgressBarMaterial && ProgressBarMaterialDynamic == nullptr)
	{
		ProgressBarMaterialDynamic = UMaterialInstanceDynamic::Create(ProgressBarMaterial, this);
		ProgressBar_Image->SetBrushFromMaterial(ProgressBarMaterialDynamic);
		return true;
	}
	return false;
}

void UObsidianRadialProgressBar::SetThickness(const float InThickness)
{
	if(ProgressBarMaterialDynamic)
	{
		ProgressBarMaterialDynamic->SetScalarParameterValue(ThicknessValueName, InThickness);
	}
}

float UObsidianRadialProgressBar::GetMappedValue(float RawValue)
{
	return FMath::GetMappedRangeValueClamped(FVector2f(0.0f, 1.0f),
		FVector2f(BarPercentMin, BarPercentMax), RawValue);
}

void UObsidianRadialProgressBar::SetPercent(float InPercent)
{
	if(ProgressBarMaterialDynamic == nullptr && !CreateProgressBarMaterial())
	{
		return;
	}
	
	BarPercent = FMath::Clamp<float>(InPercent, 0.0f, 1.0f);
	
	BarPercent = GetMappedValue(BarPercent);
	ProgressBarMaterialDynamic->SetScalarParameterValue(ProgressValueName, BarPercent);
}



// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Components/ObsidianRadialProgressBar.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"

void UObsidianRadialProgressBar::NativePreConstruct()
{
	Super::NativePreConstruct();

	CreateProgressBarMaterial();
	SetPercent(Percent);
	SetThickness(Thickness);

	Root_SizeBox->SetWidthOverride(CircularWidth);
	Root_SizeBox->SetHeightOverride(CircularHeight);
}

void UObsidianRadialProgressBar::CreateProgressBarMaterial()
{
	if(ProgressBarMaterial && ProgressBarMaterialDynamic == nullptr)
	{
		ProgressBarMaterialDynamic = UMaterialInstanceDynamic::Create(ProgressBarMaterial, this);

		ProgressBar_Image->SetBrushFromMaterial(ProgressBarMaterialDynamic);
	}
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
	float NormalizedValue = 0.0f;

	NormalizedValue = FMath::GetMappedRangeValueClamped(FVector2f(0.0f, 1.0f), FVector2f(BarPercentMin, BarPercentMax), RawValue);

	return NormalizedValue;
}

void UObsidianRadialProgressBar::SetPercent(float InPercent)
{
	if(ProgressBarMaterialDynamic)
	{
		InPercent = GetMappedValue(InPercent);
		
		ProgressBarMaterialDynamic->SetScalarParameterValue(ProgressValueName, InPercent);
	}
}



// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Components/ObsidianRadialProgressBar.h"
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

void UObsidianRadialProgressBar::NativeConstruct()
{
	Super::NativeConstruct();
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
	return FMath::GetMappedRangeValueClamped(FVector2f(0.0f, 1.0f),
		FVector2f(BarPercentMin, BarPercentMax), RawValue);
}

void UObsidianRadialProgressBar::SetPercent(float InPercent)
{
	if(ProgressBarMaterialDynamic == nullptr)
	{
		CreateProgressBarMaterial();
	}
	
	BarPercent = FMath::Clamp<float>(InPercent, 0.0f, 1.0f);
	
	BarPercent = GetMappedValue(BarPercent);
	ProgressBarMaterialDynamic->SetScalarParameterValue(ProgressValueName, BarPercent);
}



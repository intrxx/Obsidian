// Copyright 2026 out of sCope team - intrxx

#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Health.h"

#include "CommonTextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

#include "UI/Components/ObsidianRadialProgressBar.h"
#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"


void UObsidianProgressGlobe_Health::HandleWidgetControllerSet()
{
	Super::HandleWidgetControllerSet();

	MainOverlayWidgetController->OnHealthChangedDelegate.AddDynamic(this, &ThisClass::OnHealthChanged);
	MainOverlayWidgetController->OnMaxHealthChangedDelegate.AddDynamic(this, &ThisClass::OnMaxHealthChanged);
	MainOverlayWidgetController->OnEnergyShieldChangedDelegate.AddDynamic(this, &ThisClass::OnEnergyShieldChanged);
	MainOverlayWidgetController->OnMaxEnergyShieldChangedDelegate.AddDynamic(this, &ThisClass::OnMaxEnergyShieldChanged);
	MainOverlayWidgetController->OnStaggerMeterChangedDelegate.AddDynamic(this, &ThisClass::OnStaggerMeterChanged);
	MainOverlayWidgetController->OnMaxStaggerMeterChangedDelegate.AddDynamic(this, &ThisClass::OnMaxStaggerMeterChanged);
}

void UObsidianProgressGlobe_Health::SetProgressGlobeStyle(const FSlateBrush& InProgressGlobeFillImage) const
{
	if(Health_ProgressGlobe)
	{
		FProgressBarStyle Style;
		Style.BackgroundImage.TintColor = FSlateColor(FLinearColor::Transparent);
		Style.FillImage = InProgressGlobeFillImage;
		Health_ProgressGlobe->SetWidgetStyle(Style);
	}
}

void UObsidianProgressGlobe_Health::ResetStyle() const
{
	if(Health_ProgressGlobe)
	{
		FProgressBarStyle Style;
		Style.BackgroundImage.TintColor = FSlateColor(FLinearColor::Transparent);
		Style.FillImage = GlobeFillImage;
		Health_ProgressGlobe->SetWidgetStyle(Style);
	}
}

void UObsidianProgressGlobe_Health::OnHealthChanged(float InNewHealth)
{
	ShouldGhostGlobeDecrease(InNewHealth, Health, MaxHealth);
	
	Health = InNewHealth;

	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	Health_ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 HealthFloored = FMath::FloorToInt(Health);
	const int32 MaxHealthFloored = FMath::FloorToInt(MaxHealth);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), HealthFloored, MaxHealthFloored));
	HealthAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnMaxHealthChanged(float InNewMaxHealth)
{
	MaxHealth = InNewMaxHealth;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	Health_ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 HealthFloored = FMath::FloorToInt(Health);
	const int32 MaxHealthFloored = FMath::FloorToInt(MaxHealth);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), HealthFloored, MaxHealthFloored));
	HealthAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnEnergyShieldChanged(float InNewEnergyShield)
{
	EnergyShield = InNewEnergyShield;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(EnergyShield, MaxEnergyShield);
	EnergyShield_ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 EnergyShieldFloored = FMath::FloorToInt(EnergyShield);
	const int32 MaxEnergyShieldFloored = FMath::FloorToInt(MaxEnergyShield);

	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), EnergyShieldFloored, MaxEnergyShieldFloored));
	EnergyShieldAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnMaxEnergyShieldChanged(float InNewMaxEnergyShield)
{
	MaxEnergyShield = InNewMaxEnergyShield;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(EnergyShield, MaxEnergyShield);
	EnergyShield_ProgressGlobe->SetPercent(ProgressBarPercent);
	
	const int32 EnergyShieldFloored = FMath::FloorToInt(EnergyShield);
	const int32 MaxEnergyShieldFloored = FMath::FloorToInt(MaxEnergyShield);

	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), EnergyShieldFloored, MaxEnergyShieldFloored));
	EnergyShieldAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnStaggerMeterChanged(float InNewStaggerMeter)
{
	StaggerMeter = InNewStaggerMeter;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(StaggerMeter, MaxStaggerMeter);
	Stagger_RadialProgressBar->SetPercent(ProgressBarPercent);
}

void UObsidianProgressGlobe_Health::OnMaxStaggerMeterChanged(float InNewMaxStaggerMeter)
{
	MaxStaggerMeter = InNewMaxStaggerMeter;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(StaggerMeter, MaxStaggerMeter);
	Stagger_RadialProgressBar->SetPercent(ProgressBarPercent);
}

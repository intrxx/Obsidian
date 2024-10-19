// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Health.h"
#include "CommonTextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

void UObsidianProgressGlobe_Health::SetProgressGlobeStyle(const FSlateBrush& ProgressGlobeFillImage) const
{
	if(Health_ProgressGlobe)
	{
		FProgressBarStyle Style;
		Style.BackgroundImage.TintColor = FSlateColor(FLinearColor::Transparent);
		Style.FillImage = ProgressGlobeFillImage;
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

void UObsidianProgressGlobe_Health::HandleWidgetControllerSet()
{
	Super::HandleWidgetControllerSet();

	MainOverlayWidgetController->OnHealthChangedDelegate.AddDynamic(this, &ThisClass::OnHealthChanged);
	MainOverlayWidgetController->OnMaxHealthChangedDelegate.AddDynamic(this, &ThisClass::OnMaxHealthChanged);
	MainOverlayWidgetController->OnEnergyShieldChangedDelegate.AddDynamic(this, &ThisClass::OnEnergyShieldChanged);
	MainOverlayWidgetController->OnMaxEnergyShieldChangedDelegate.AddDynamic(this, &ThisClass::OnMaxEnergyShieldChanged);
}

void UObsidianProgressGlobe_Health::OnHealthChanged(float NewHealth)
{
	ShouldGhostGlobeDecrease(NewHealth, Health, MaxHealth);
	
	Health = NewHealth;

	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	Health_ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 HealthFloored = FMath::FloorToInt(Health);
	const int32 MaxHealthFloored = FMath::FloorToInt(MaxHealth);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), HealthFloored, MaxHealthFloored));
	HealthAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnMaxHealthChanged(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	Health_ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 HealthFloored = FMath::FloorToInt(Health);
	const int32 MaxHealthFloored = FMath::FloorToInt(MaxHealth);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), HealthFloored, MaxHealthFloored));
	HealthAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnEnergyShieldChanged(float NewEnergyShield)
{
	EnergyShield = NewEnergyShield;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(EnergyShield, MaxEnergyShield);
	EnergyShield_ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 EnergyShieldFloored = FMath::FloorToInt(EnergyShield);
	const int32 MaxEnergyShieldFloored = FMath::FloorToInt(MaxEnergyShield);

	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), EnergyShieldFloored, MaxEnergyShieldFloored));
	EnergyShieldAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnMaxEnergyShieldChanged(float NewMaxEnergyShield)
{
	MaxEnergyShield = NewMaxEnergyShield;

	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(EnergyShield, MaxEnergyShield);
	EnergyShield_ProgressGlobe->SetPercent(ProgressBarPercent);
	
	const int32 EnergyShieldFloored = FMath::FloorToInt(EnergyShield);
	const int32 MaxEnergyShieldFloored = FMath::FloorToInt(MaxEnergyShield);

	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), EnergyShieldFloored, MaxEnergyShieldFloored));
	EnergyShieldAttributeCount_TextBlock->SetText(AttributeText);
}

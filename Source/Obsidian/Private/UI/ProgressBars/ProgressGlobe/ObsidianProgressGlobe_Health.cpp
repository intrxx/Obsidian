// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Health.h"
#include "CommonTextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

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
	ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 HealthFloored = FMath::FloorToInt(Health);
	const int32 MaxHealthFloored = FMath::FloorToInt(MaxHealth);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), HealthFloored, MaxHealthFloored));
	FirstAttributeCountText->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnMaxHealthChanged(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
	
	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 HealthFloored = FMath::FloorToInt(Health);
	const int32 MaxHealthFloored = FMath::FloorToInt(MaxHealth);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), HealthFloored, MaxHealthFloored));
	FirstAttributeCountText->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnEnergyShieldChanged(float NewEnergyShield)
{
	EnergyShield = NewEnergyShield;

	const int32 EnergyShieldFloored = FMath::FloorToInt(EnergyShield);
	const int32 MaxEnergyShieldFloored = FMath::FloorToInt(MaxEnergyShield);

	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), EnergyShieldFloored, MaxEnergyShieldFloored));
	SecondAttributeCountText->SetText(AttributeText);
}

void UObsidianProgressGlobe_Health::OnMaxEnergyShieldChanged(float NewMaxEnergyShield)
{
	MaxEnergyShield = NewMaxEnergyShield;
	
	const int32 EnergyShieldFloored = FMath::FloorToInt(EnergyShield);
	const int32 MaxEnergyShieldFloored = FMath::FloorToInt(MaxEnergyShield);

	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), EnergyShieldFloored, MaxEnergyShieldFloored));
	SecondAttributeCountText->SetText(AttributeText);
}

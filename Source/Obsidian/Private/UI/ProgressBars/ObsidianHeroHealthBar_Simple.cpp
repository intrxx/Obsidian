// Copyright 2026 out of sCope team - intrxx


#include "UI/ProgressBars/ObsidianHeroHealthBar_Simple.h"

#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"


void UObsidianHeroHealthBar_Simple::HandleWidgetControllerSet()
{
	UObsidianHeroAttributesComponent* HeroAttributesComp = Cast<UObsidianHeroAttributesComponent>(WidgetController);
	if(HeroAttributesComp == nullptr)
	{
		return;
	}
	
	HeroAttributesComp->OnHeroHealthChangedDelegate.AddUObject(this, &ThisClass::HealthChanged);
	HeroAttributesComp->OnHeroMaxHealthChangedDelegate.AddUObject(this, &ThisClass::MaxHealthChanged);
	HeroAttributesComp->OnHeroEnergyShieldChangedDelegate.AddUObject(this, &ThisClass::EnergyShieldChanged);
	HeroAttributesComp->OnHeroMaxEnergyShieldChangedDelegate.AddUObject(this, &ThisClass::MaxEnergyShieldChanged);

	if(!HeroAttributesComp->IsDeadOrDying())
	{
		Health = HeroAttributesComp->GetHealth();
		MaxHealth = HeroAttributesComp->GetMaxHealth();
		EnergyShield = HeroAttributesComp->GetEnergyShield();
		MaxEnergyShield = HeroAttributesComp->GetMaxEnergyShield();
		
		SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
		SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
	}
}

void UObsidianHeroHealthBar_Simple::HealthChanged(const float InNewHealth)
{
	Health = InNewHealth;
	SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
}

void UObsidianHeroHealthBar_Simple::MaxHealthChanged(const float InNewMaxHealth)
{
	MaxHealth = InNewMaxHealth;
	SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
}

void UObsidianHeroHealthBar_Simple::EnergyShieldChanged(const float InNewEnergyShield)
{
	EnergyShield = InNewEnergyShield;
	SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}

void UObsidianHeroHealthBar_Simple::MaxEnergyShieldChanged(const float InNewMaxEnergyShield)
{
	MaxEnergyShield = InNewMaxEnergyShield;
	SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}
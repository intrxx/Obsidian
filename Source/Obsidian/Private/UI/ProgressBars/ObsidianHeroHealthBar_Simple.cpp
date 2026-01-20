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

void UObsidianHeroHealthBar_Simple::HealthChanged(const float NewHealth)
{
	Health = NewHealth;
	SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
}

void UObsidianHeroHealthBar_Simple::MaxHealthChanged(const float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
	SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
}

void UObsidianHeroHealthBar_Simple::EnergyShieldChanged(const float NewEnergyShield)
{
	EnergyShield = NewEnergyShield;
	SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}

void UObsidianHeroHealthBar_Simple::MaxEnergyShieldChanged(const float NewMaxEnergyShield)
{
	MaxEnergyShield = NewMaxEnergyShield;
	SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}
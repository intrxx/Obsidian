// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ProgressBars/ObsidianHeroHealthBar.h"

// ~ Core

// ~ Project
#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"

void UObsidianHeroHealthBar::HandleWidgetControllerSet()
{
   UObMainOverlayWidgetController* WC = Cast<UObMainOverlayWidgetController>(WidgetController);
   if(WC == nullptr)
   {
      return;
   }

   WC->OnHealthChangedDelegate.AddDynamic(this, &ThisClass::HealthChanged);
   WC->OnMaxHealthChangedDelegate.AddDynamic(this, &ThisClass::MaxHealthChanged);
   WC->OnEnergyShieldChangedDelegate.AddDynamic(this, &ThisClass::EnergyShieldChanged);
   WC->OnMaxEnergyShieldChangedDelegate.AddDynamic(this, &ThisClass::MaxEnergyShieldChanged);
   WC->OnManaChangedDelegate.AddDynamic(this, &ThisClass::ManaChanged);
   WC->OnMaxManaChangedDelegate.AddDynamic(this, &ThisClass::MaxManaChanged);

   WC->SetInitialAttributeValues();
}

void UObsidianHeroHealthBar::HealthChanged(const float NewHealth)
{
   Health = NewHealth;
   SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
}

void UObsidianHeroHealthBar::MaxHealthChanged(const float NewMaxHealth)
{
   MaxHealth = NewMaxHealth;
   SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
}

void UObsidianHeroHealthBar::EnergyShieldChanged(const float NewEnergyShield)
{
   EnergyShield = NewEnergyShield;
   SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}

void UObsidianHeroHealthBar::MaxEnergyShieldChanged(const float NewMaxEnergyShield)
{
   MaxEnergyShield = NewMaxEnergyShield;
   SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}

void UObsidianHeroHealthBar::ManaChanged(const float NewMana)
{
   Mana = NewMana;
   SetProgressBarPercent(Mana, MaxMana, Mana_ProgressBar);
}

void UObsidianHeroHealthBar::MaxManaChanged(const float NewMaxMana)
{
   MaxMana = NewMaxMana;
   SetProgressBarPercent(Mana, MaxMana, Mana_ProgressBar);
}

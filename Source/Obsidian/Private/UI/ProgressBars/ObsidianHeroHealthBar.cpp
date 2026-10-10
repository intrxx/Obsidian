// Copyright 2026 out of sCope team - intrxx

#include "UI/ProgressBars/ObsidianHeroHealthBar.h"

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

void UObsidianHeroHealthBar::HealthChanged(const float InNewHealth)
{
   Health = InNewHealth;
   SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
}

void UObsidianHeroHealthBar::MaxHealthChanged(const float InNewMaxHealth)
{
   MaxHealth = InNewMaxHealth;
   SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
}

void UObsidianHeroHealthBar::EnergyShieldChanged(const float InNewEnergyShield)
{
   EnergyShield = InNewEnergyShield;
   SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}

void UObsidianHeroHealthBar::MaxEnergyShieldChanged(const float InNewMaxEnergyShield)
{
   MaxEnergyShield = InNewMaxEnergyShield;
   SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}

void UObsidianHeroHealthBar::ManaChanged(const float InNewMana)
{
   Mana = InNewMana;
   SetProgressBarPercent(Mana, MaxMana, Mana_ProgressBar);
}

void UObsidianHeroHealthBar::MaxManaChanged(const float InNewMaxMana)
{
   MaxMana = InNewMaxMana;
   SetProgressBarPercent(Mana, MaxMana, Mana_ProgressBar);
}

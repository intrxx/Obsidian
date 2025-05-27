// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ProgressBars/ObsidianRegularEnemyHealthBar.h"

// ~ Core

// ~ Project
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"

void UObsidianRegularEnemyHealthBar::HandleWidgetControllerSet()
{
   UObsidianEnemyAttributesComponent* EnemyAttributesComp = Cast<UObsidianEnemyAttributesComponent>(WidgetController);
   if(EnemyAttributesComp == nullptr)
   {
      return;
   }
   
   EnemyAttributesComp->HealthChangedDelegate.AddUObject(this, &ThisClass::HealthChanged);
   EnemyAttributesComp->MaxHealthChangedDelegate.AddUObject(this, &ThisClass::MaxHealthChanged);
   EnemyAttributesComp->EnergyShieldChangedDelegate.AddUObject(this, &ThisClass::EnergyShieldChanged);
   EnemyAttributesComp->MaxEnergyShieldChangedDelegate.AddUObject(this, &ThisClass::MaxEnergyShieldChanged);

   if(!EnemyAttributesComp->IsDeadOrDying())
   {
      Health = EnemyAttributesComp->GetHealth();
      MaxHealth = EnemyAttributesComp->GetMaxHealth();
      EnergyShield = EnemyAttributesComp->GetEnergyShield();
      MaxEnergyShield = EnemyAttributesComp->GetMaxEnergyShield();
		
      SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
      SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
   }
}

void UObsidianRegularEnemyHealthBar::HealthChanged(const float NewValue)
{
   Health = NewValue;
   SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
   StartWidgetHideTimer();
}

void UObsidianRegularEnemyHealthBar::MaxHealthChanged(const float NewValue)
{
   MaxHealth = NewValue;
   SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
   StartWidgetHideTimer();
}

void UObsidianRegularEnemyHealthBar::EnergyShieldChanged(const float NewValue)
{
   EnergyShield = NewValue;
   SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
   StartWidgetHideTimer();
}

void UObsidianRegularEnemyHealthBar::MaxEnergyShieldChanged(const float NewValue)
{
   MaxEnergyShield = NewValue;
   SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
   StartWidgetHideTimer();
}

//TODO: This should probably be an option in the future
void UObsidianRegularEnemyHealthBar::StartWidgetHideTimer()
{
   if(!bShouldHideWidget)
   {
      return;
   }
   
   SetVisibility(ESlateVisibility::Visible);
   
   const UWorld* World = GetWorld();
   if(World == nullptr)
   {
      return;
   }

   if(World->GetTimerManager().TimerExists(HideTimerHandle))
   {
      World->GetTimerManager().ClearTimer(HideTimerHandle);
   }
   World->GetTimerManager().SetTimer(HideTimerHandle, this, &ThisClass::HideWidget, HideTime);
}

void UObsidianRegularEnemyHealthBar::HideWidget()
{
   SetVisibility(ESlateVisibility::Hidden);
}



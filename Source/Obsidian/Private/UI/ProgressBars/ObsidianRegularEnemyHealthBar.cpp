// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianRegularEnemyHealthBar.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"

void UObsidianRegularEnemyHealthBar::HandleWidgetControllerSet()
{
   UObsidianEnemyAttributesComponent* EnemyAttributesComp = Cast<UObsidianEnemyAttributesComponent>(WidgetController);
   if(EnemyAttributesComp == nullptr)
   {
      return;
   }
   
   EnemyAttributesComp->HealthChangedDelegate.AddLambda([this](const float NewValue)
   {
      Health = NewValue;
      SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
      StartWidgetHideTimer();
   });

   EnemyAttributesComp->MaxHealthChangedDelegate.AddLambda([this](const float NewValue)
   {
      MaxHealth = NewValue;
      SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
      StartWidgetHideTimer();
   });

   EnemyAttributesComp->EnergyShieldChangedDelegate.AddLambda([this](const float NewValue)
   {
      EnergyShield = NewValue;
      SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
      StartWidgetHideTimer();
   });
   
   EnemyAttributesComp->MaxEnergyShieldChangedDelegate.AddLambda([this](const float NewValue)
   {
      MaxEnergyShield = NewValue;
      SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
      StartWidgetHideTimer();
   });
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


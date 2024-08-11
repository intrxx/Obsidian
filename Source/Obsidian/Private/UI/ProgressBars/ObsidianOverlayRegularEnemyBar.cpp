// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianOverlayRegularEnemyBar.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"

void UObsidianOverlayRegularEnemyBar::HandleWidgetControllerSet()
{
	UObsidianEnemyAttributesComponent* EnemyAttributesComp = Cast<UObsidianEnemyAttributesComponent>(WidgetController);
	if(EnemyAttributesComp == nullptr)
	{
		return;
	}
   
	EnemyAttributesComp->HealthChangedDelegate.AddLambda([this, EnemyAttributesComp](const float NewValue)
	{
		if(!EnemyAttributesComp->IsDeadOrDying())
		{
			Health = NewValue;
			SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
		}
	});

	EnemyAttributesComp->MaxHealthChangedDelegate.AddLambda([this](const float NewValue)
	{
	    MaxHealth = NewValue;
	    SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
	});

	EnemyAttributesComp->EnergyShieldChangedDelegate.AddLambda([this](const float NewValue)
	{
	    EnergyShield = NewValue;
	    SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
	});
   
	EnemyAttributesComp->MaxEnergyShieldChangedDelegate.AddLambda([this](const float NewValue)
	{
	    MaxEnergyShield = NewValue;
	    SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
	});

	SetInitialValues(EnemyAttributesComp);
}

void UObsidianOverlayRegularEnemyBar::SetInitialValues(const UObsidianEnemyAttributesComponent* EnemyAttributesComp) const
{
	if(!EnemyAttributesComp->IsDeadOrDying())
	{
		SetProgressBarPercent(EnemyAttributesComp->GetHealth(), EnemyAttributesComp->GetMaxHealth(), Health_ProgressBar);
		SetProgressBarPercent(EnemyAttributesComp->GetEnergyShield(), EnemyAttributesComp->GetMaxEnergyShield(), EnergyShield_ProgressBar);
	}
}

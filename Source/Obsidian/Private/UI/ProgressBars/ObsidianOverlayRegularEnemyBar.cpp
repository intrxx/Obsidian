// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/ObsidianOverlayRegularEnemyBar.h"

#include "CommonTextBlock.h"
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

	if(EnemyName_TextBlock)
	{
		EnemyName_TextBlock->SetText(EnemyAttributesComp->GetEnemyName());
	}
}

void UObsidianOverlayRegularEnemyBar::SetInitialValues(const UObsidianEnemyAttributesComponent* EnemyAttributesComp)
{
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

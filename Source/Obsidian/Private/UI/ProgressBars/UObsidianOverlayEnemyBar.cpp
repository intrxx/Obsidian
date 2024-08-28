// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/UObsidianOverlayEnemyBar.h"
#include "CommonTextBlock.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"

void UObsidianOverlayEnemyBar::HandleWidgetControllerSet()
{
	UObsidianEnemyAttributesComponent* EnemyAttributesComp = Cast<UObsidianEnemyAttributesComponent>(WidgetController);
	if(EnemyAttributesComp == nullptr)
	{
		return;
	}
   
	HealthChangedDelegateHandle = EnemyAttributesComp->HealthChangedDelegate.AddLambda([this, EnemyAttributesComp](const float NewValue)
	{
		if(!EnemyAttributesComp->IsDeadOrDying())
		{
			Health = NewValue;
			SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);

			if(Health <= 0)
			{
				UninitAndHide();
			}
		}
	});

	MaxHealthChangedDelegateHandle = EnemyAttributesComp->MaxHealthChangedDelegate.AddLambda([this](const float NewValue)
	{
	    MaxHealth = NewValue;
	    SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
	});

	EnergyShieldChangedDelegateHandle = EnemyAttributesComp->EnergyShieldChangedDelegate.AddLambda([this](const float NewValue)
	{
	    EnergyShield = NewValue;
	    SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
	});
   
	MaxEnergyShieldChangedDelegateHandle = EnemyAttributesComp->MaxEnergyShieldChangedDelegate.AddLambda([this](const float NewValue)
	{
	    MaxEnergyShield = NewValue;
	    SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
	});

	StaggerMeterChangedDelegateHandle = EnemyAttributesComp->StaggerMeterChangedDelegate.AddLambda([this](const float NewValue)
	{
		StaggerMeter = NewValue;
		SetProgressBarPercent(StaggerMeter, MaxStaggerMeter, StaggerMeter_ProgressBar);
	});

	MaxStaggerMeterChangedDelegateHandle = EnemyAttributesComp->MaxStaggerMeterChangedDelegate.AddLambda([this](const float NewValue)
	{
		MaxStaggerMeter = NewValue;
		SetProgressBarPercent(StaggerMeter, MaxStaggerMeter, StaggerMeter_ProgressBar);
	});

	SetInitialValues(EnemyAttributesComp);

	if(EnemyName_TextBlock)
	{
		EnemyName_TextBlock->SetText(EnemyAttributesComp->GetEnemyName());
	}
}

void UObsidianOverlayEnemyBar::SetInitialValues(const UObsidianEnemyAttributesComponent* EnemyAttributesComp)
{
	if(!EnemyAttributesComp->IsDeadOrDying())
	{
		Health = EnemyAttributesComp->GetHealth();
		MaxHealth = EnemyAttributesComp->GetMaxHealth();
		EnergyShield = EnemyAttributesComp->GetEnergyShield();
		MaxEnergyShield = EnemyAttributesComp->GetMaxEnergyShield();
		StaggerMeter = EnemyAttributesComp->GetStaggerMeter();
		MaxStaggerMeter = EnemyAttributesComp->GetMaxStaggerMeter();
		
		SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
		SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
		SetProgressBarPercent(StaggerMeter, MaxStaggerMeter, StaggerMeter_ProgressBar);
	}
}

void UObsidianOverlayEnemyBar::UninitAndHide()
{
	SetVisibility(ESlateVisibility::Collapsed);

	HealthChangedDelegateHandle.Reset();
	MaxHealthChangedDelegateHandle.Reset();
	EnergyShieldChangedDelegateHandle.Reset();
	MaxEnergyShieldChangedDelegateHandle.Reset();
	StaggerMeterChangedDelegateHandle.Reset();
	MaxStaggerMeterChangedDelegateHandle.Reset();
}

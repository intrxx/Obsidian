// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/UObsidianOverlayEnemyBar.h"
#include "CommonTextBlock.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"

void UObsidianOverlayEnemyBar::HandleWidgetControllerSet()
{
	EnemyAttributesComp = Cast<UObsidianEnemyAttributesComponent>(WidgetController);
	if(EnemyAttributesComp == nullptr)
	{
		return;
	}
   
	HealthChangedDelegateHandle = EnemyAttributesComp->HealthChangedDelegate.AddUObject(this, &ThisClass::HealthChanged);
	MaxHealthChangedDelegateHandle = EnemyAttributesComp->MaxHealthChangedDelegate.AddUObject(this, &ThisClass::MaxHealthChanged);
	EnergyShieldChangedDelegateHandle = EnemyAttributesComp->EnergyShieldChangedDelegate.AddUObject(this, &ThisClass::EnergyShieldChanged);
	MaxEnergyShieldChangedDelegateHandle = EnemyAttributesComp->MaxEnergyShieldChangedDelegate.AddUObject(this, &ThisClass::MaxEnergyShieldChanged);
	StaggerMeterChangedDelegateHandle = EnemyAttributesComp->StaggerMeterChangedDelegate.AddUObject(this, &ThisClass::StaggerMeterChanged);
	MaxStaggerMeterChangedDelegateHandle = EnemyAttributesComp->MaxStaggerMeterChangedDelegate.AddUObject(this, &ThisClass::MaxStaggerMeterChanged);

	SetInitialValues();

	bIsBind = EnemyAttributesComp->BindToOnEffectCallback();
	if(bIsBind)
	{
		EnemyAttributesComp->EffectUIDataWidgetRowDelegate.AddDynamic(this, &ThisClass::HandleUIData);
		EnemyAttributesComp->EffectStackingUIDataDelegate.AddDynamic(this, &ThisClass::HandleStackingUIData);
	}
	
	if(EnemyName_TextBlock)
	{
		const FText EnemyName = EnemyAttributesComp->GetEnemyName();
		EnemyName_TextBlock->SetText(EnemyName);
	}
}

void UObsidianOverlayEnemyBar::SetInitialValues()
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

void UObsidianOverlayEnemyBar::HandleUIData(const FObsidianEffectUIDataWidgetRow Row)
{
	UE_LOG(LogTemp, Warning, TEXT("Effect [%s] on Enemy [%s]"), *Row.EffectName.ToString(), *EnemyAttributesComp->GetEnemyName().ToString());
}

void UObsidianOverlayEnemyBar::HandleStackingUIData(const FObsidianEffectUIDataWidgetRow Row, const FObsidianEffectUIStackingData StackingData)
{
	UE_LOG(LogTemp, Warning, TEXT("Effect [%s] on Enemy [%s]"), *Row.EffectName.ToString(), *EnemyAttributesComp->GetEnemyName().ToString());
}

void UObsidianOverlayEnemyBar::HealthChanged(const float NewValue)
{
	if(EnemyAttributesComp && !EnemyAttributesComp->IsDeadOrDying())
	{
		Health = NewValue;
		SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);

		if(Health <= 0)
		{
			UninitAndDestroy();
		}
	}
}

void UObsidianOverlayEnemyBar::MaxHealthChanged(const float NewValue)
{
	MaxHealth = NewValue;
	SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
}

void UObsidianOverlayEnemyBar::EnergyShieldChanged(const float NewValue)
{
	EnergyShield = NewValue;
	SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}

void UObsidianOverlayEnemyBar::MaxEnergyShieldChanged(const float NewValue)
{
	MaxEnergyShield = NewValue;
	SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
}

void UObsidianOverlayEnemyBar::StaggerMeterChanged(const float NewValue)
{
	StaggerMeter = NewValue;
	SetProgressBarPercent(StaggerMeter, MaxStaggerMeter, StaggerMeter_ProgressBar);
}

void UObsidianOverlayEnemyBar::MaxStaggerMeterChanged(const float NewValue)
{
	MaxStaggerMeter = NewValue;
	SetProgressBarPercent(StaggerMeter, MaxStaggerMeter, StaggerMeter_ProgressBar);
}

void UObsidianOverlayEnemyBar::UninitAndDestroy()
{
	HealthChangedDelegateHandle.Reset();
	MaxHealthChangedDelegateHandle.Reset();
	EnergyShieldChangedDelegateHandle.Reset();
	MaxEnergyShieldChangedDelegateHandle.Reset();
	StaggerMeterChangedDelegateHandle.Reset();
	MaxStaggerMeterChangedDelegateHandle.Reset();

	if(EnemyAttributesComp && bIsBind)
	{
		EnemyAttributesComp->ClearOnEffectCallback();
	}

	RemoveFromParent();
}

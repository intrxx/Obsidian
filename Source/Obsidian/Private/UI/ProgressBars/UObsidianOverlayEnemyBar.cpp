// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ProgressBars/UObsidianOverlayEnemyBar.h"
#include "CommonTextBlock.h"
#include "CharacterComponents/ObsidianEnemyOverlayBarComponent.h"
#include "Components/ProgressBar.h"

void UObsidianOverlayEnemyBar::ResetStyle() const
{
	if(Health_ProgressBar)
	{
		FProgressBarStyle Style;
		Style.BackgroundImage.TintColor = FSlateColor(FLinearColor::Transparent);
		Style.FillImage = OverlayBarFillImage;
		Health_ProgressBar->SetWidgetStyle(Style);
	}
}

void UObsidianOverlayEnemyBar::NativePreConstruct()
{
	Super::NativePreConstruct();

	ResetStyle();
}

void UObsidianOverlayEnemyBar::HandleWidgetControllerSet()
{
	EnemyOverlayBarComp = Cast<UObsidianEnemyOverlayBarComponent>(WidgetController);
	if(EnemyOverlayBarComp == nullptr)
	{
		return;
	}
   
	EnemyOverlayBarComp->OnHealthChangedDelegate.BindUObject(this, &ThisClass::HealthChanged);
	EnemyOverlayBarComp->OnMaxHealthChangedDelegate.BindUObject(this, &ThisClass::MaxHealthChanged);
	EnemyOverlayBarComp->OnEnergyShieldChangedDelegate.BindUObject(this, &ThisClass::EnergyShieldChanged);
	EnemyOverlayBarComp->OnMaxEnergyShieldChangedDelegate.BindUObject(this, &ThisClass::MaxEnergyShieldChanged);
	EnemyOverlayBarComp->OnStaggerMeterChangedDelegate.BindUObject(this, &ThisClass::StaggerMeterChanged);
	EnemyOverlayBarComp->OnMaxStaggerMeterChangedDelegate.BindUObject(this, &ThisClass::MaxStaggerMeterChanged);

	SetInitialValues();

	EnemyOverlayBarComp->OnOverlayBarStyleResetDelegate.AddUObject(this, &ThisClass::ResetStyle);
	EnemyOverlayBarComp->OnNewOverlayBarStyleNeededDelegate.AddUObject(this, &ThisClass::SetOverlayBarStyle);
	
	if(EnemyName_TextBlock)
	{
		const FText EnemyName = EnemyOverlayBarComp->GetEnemyName();
		EnemyName_TextBlock->SetText(EnemyName);
	}
}

void UObsidianOverlayEnemyBar::SetInitialValues()
{
	if(!EnemyOverlayBarComp->IsDeadOrDying())
	{
		EnemyOverlayBarComp->FillInitialValues(Health, MaxHealth, EnergyShield,
			MaxEnergyShield, StaggerMeter, MaxStaggerMeter);
		
		SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);
		SetProgressBarPercent(EnergyShield, MaxEnergyShield, EnergyShield_ProgressBar);
		SetProgressBarPercent(StaggerMeter, MaxStaggerMeter, StaggerMeter_ProgressBar);
	}
}

void UObsidianOverlayEnemyBar::SetOverlayBarStyle(const FSlateBrush& Brush) const
{
	if(Health_ProgressBar)
	{
		FProgressBarStyle Style;
		Style.BackgroundImage.TintColor = FSlateColor(FLinearColor::Transparent);
		Style.FillImage = Brush;
		Health_ProgressBar->SetWidgetStyle(Style);
	}
}

void UObsidianOverlayEnemyBar::HealthChanged(const float NewValue)
{
	if(EnemyOverlayBarComp && !EnemyOverlayBarComp->IsDeadOrDying())
	{
		Health = NewValue;
		SetProgressBarPercent(Health, MaxHealth, Health_ProgressBar);

		if(Health <= 0)
		{
			RemoveFromParent();
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


// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/OCharacterStatusWidgetController.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Player/ObsidianPlayerState.h"

void UOCharacterStatusWidgetController::OnWidgetControllerSetupCompleted()
{
	check(ObsidianAbilitySystemComponent);
	check(AttributesComponent);
	check(ObsidianPlayerState);
	
	HandleBindingCallbacks(ObsidianAbilitySystemComponent);

	ObsidianPlayerState->OnHeroLevelUp.AddDynamic(this, &ThisClass::HeroLevelUp);
}

void UOCharacterStatusWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
	/** Character */
	ExperienceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetExperienceAttribute()).AddUObject(this, &ThisClass::ExperienceChanged);
	MaxExperienceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxExperienceAttribute()).AddUObject(this, &ThisClass::MaxExperienceChanged);
	
	/** Attributes */
	StrengthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetStrengthAttribute()).AddUObject(this, &ThisClass::StrengthChanged);
	IntelligenceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetIntelligenceAttribute()).AddUObject(this, &ThisClass::IntelligenceChanged);
	DexterityChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetDexterityAttribute()).AddUObject(this, &ThisClass::DexterityChanged);
	FaithChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetFaithAttribute()).AddUObject(this, &ThisClass::FaithChanged);
	
	/** Vital Attributes */
	MaxHealthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	MaxEnergyShieldChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxEnergyShieldAttribute()).AddUObject(this, &ThisClass::MaxEnergyShieldChanged);
	MaxSpecialResourceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxSpecialResourceAttribute()).AddUObject(this,  &ThisClass::MaxSpecialResourceChanged);
	MaxManaChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	
	/** Offence */
	AccuracyChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetAccuracyAttribute()).AddUObject(this, &ThisClass::AccuracyChanged);
	AttackSpeedChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetAttackSpeedAttribute()).AddUObject(this, &ThisClass::AttackSpeedChanged);
	CastSpeedChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetCastSpeedAttribute()).AddUObject(this, &ThisClass::CastSpeedChanged);
	CriticalStrikeChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetCriticalStrikeChanceAttribute()).AddUObject(this, &ThisClass::CriticalStrikeChanceChanged);
	CriticalStrikeDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetCriticalStrikeDamageMultiplierAttribute()).AddUObject(this, &ThisClass::CriticalStrikeDamageMultiplierChanged);
	PhysicalDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetPhysicalDamageMultiplierAttribute()).AddUObject(this, &ThisClass::PhysicalDamageMultiplierChanged);
	FireDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetFireDamageMultiplierAttribute()).AddUObject(this, &ThisClass::FireDamageMultiplierChanged);
	LightningDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetLightningDamageMultiplierAttribute()).AddUObject(this, &ThisClass::LightningDamageMultiplierChanged);
	ColdDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetColdDamageMultiplierAttribute()).AddUObject(this, &ThisClass::ColdDamageMultiplierChanged);
	ChaosDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetChaosDamageMultiplierAttribute()).AddUObject(this, &ThisClass::ChaosDamageMultiplierChanged);
	FirePenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetFirePenetrationAttribute()).AddUObject(this, &ThisClass::FirePenetrationChanged);
	LightningPenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetLightningPenetrationAttribute()).AddUObject(this, &ThisClass::LightningPenetrationChanged);
	ColdPenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetColdPenetrationAttribute()).AddUObject(this, &ThisClass::ColdPenetrationChanged);
	ChaosPenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetChaosPenetrationAttribute()).AddUObject(this, &ThisClass::ChaosPenetrationChanged);
	
	/** Defence */
	ArmorChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetArmorAttribute()).AddUObject(this, &ThisClass::ArmorChanged);
	EvasionChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetEvasionAttribute()).AddUObject(this, &ThisClass::EvasionChanged);
	HealthRegenerationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetHealthRegenerationAttribute()).AddUObject(this, &ThisClass::HealthRegenerationChanged);
	EnergyShieldRegenerationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetEnergyShieldRegenerationAttribute()).AddUObject(this, &ThisClass::EnergyShieldRegenerationChanged);
	FireResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetFireResistanceAttribute()).AddUObject(this, &ThisClass::FireResistanceChanged);
	MaxFireResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxFireResistanceAttribute()).AddUObject(this, &ThisClass::MaxFireResistanceChanged);
	ColdResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetColdResistanceAttribute()).AddUObject(this, &ThisClass::ColdResistanceChanged);
	MaxColdResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxColdResistanceAttribute()).AddUObject(this, &ThisClass::MaxColdResistanceChanged);
	LightningResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetLightningResistanceAttribute()).AddUObject(this, &ThisClass::LightningResistanceChanged);
	MaxLightningResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxLightningResistanceAttribute()).AddUObject(this, &ThisClass::MaxLightningResistanceChanged);
	ChaosResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetChaosResistanceAttribute()).AddUObject(this, &ThisClass::ChaosResistanceChanged);
	MaxChaosResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxChaosResistanceAttribute()).AddUObject(this, &ThisClass::MaxChaosResistanceChanged);
	SpellSuppressionChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetSpellSuppressionChanceAttribute()).AddUObject(this, &ThisClass::SpellSuppressionChanceChanged);
	SpellSuppressionMagnitudeChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetSpellSuppressionMagnitudeAttribute()).AddUObject(this, &ThisClass::SpellSuppressionMagnitudeChanged);
	HitBlockChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetHitBlockChanceAttribute()).AddUObject(this, &ThisClass::HitBlockChanceChanged);
	MaxHitBlockChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxHitBlockChanceAttribute()).AddUObject(this, &ThisClass::MaxHitBlockChanceChanged);
	SpellBlockChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetSpellBlockChanceAttribute()).AddUObject(this, &ThisClass::SpellBlockChanceChanged);
	MaxSpellBlockChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxSpellBlockChanceAttribute()).AddUObject(this, &ThisClass::MaxSpellBlockChanceChanged);
}

void UOCharacterStatusWidgetController::SetInitialAttributeValues() const
{
	/** Character */
	HeroLevelUpDelegate.Broadcast(ObsidianPlayerState->GetHeroLevel());
	ExperienceChangedDelegate.Execute(AttributesComponent->GetExperience());
	MaxExperienceChangedDelegate.Execute(AttributesComponent->GetMaxExperience(), MaxExperienceOldValue);
	
	/** Attributes */
	StrengthValueChangedDelegate.Execute(AttributesComponent->GetStrength());
	IntelligenceValueChangedDelegate.Execute(AttributesComponent->GetIntelligence());
	DexterityValueChangedDelegate.Execute(AttributesComponent->GetDexterity());
	FaithValueChangedDelegate.Execute(AttributesComponent->GetFaith());

	/** Vital Attributes */
	MaxHealthChangedDelegate.Execute(AttributesComponent->GetMaxHealth());
	MaxManaChangedDelegate.Execute(AttributesComponent->GetMaxMana());
	MaxSpecialResourceChangedDelegate.Execute(AttributesComponent->GetMaxSpecialResource());
	MaxEnergyShieldChangedDelegate.Execute(AttributesComponent->GetMaxEnergyShield());
	
	/** Offence */
	AccuracyChangedDelegate.Execute(AttributesComponent->GetAccuracy());
	AttackSpeedChangedDelegate.Execute(AttributesComponent->GetAttackSpeed());
	CastSpeedChangedDelegate.Execute(AttributesComponent->GetCastSpeed());
	CriticalStrikeChanceChangedDelegate.Execute(AttributesComponent->GetCriticalStrikeChance());
	CriticalStrikeDamageMultiplierChangedDelegate.Execute(AttributesComponent->GetCriticalStrikeDamageMultiplier());
	PhysicalDamageMultiplierChangedDelegate.Execute(AttributesComponent->GetPhysicalDamageMultiplier());
	FireDamageMultiplierChangedDelegate.Execute(AttributesComponent->GetFireDamageMultiplier());
	LightningDamageMultiplierChangedDelegate.Execute(AttributesComponent->GetLightningDamageMultiplier());
	ColdDamageMultiplierChangedDelegate.Execute(AttributesComponent->GetColdDamageMultiplier());
	ChaosDamageMultiplierChangedDelegate.Execute(AttributesComponent->GetChaosDamageMultiplier());
	FirePenetrationChangedDelegate.Execute(AttributesComponent->GetFirePenetration());
	LightningPenetrationChangedDelegate.Execute(AttributesComponent->GetLightningPenetration());
	ColdPenetrationChangedDelegate.Execute(AttributesComponent->GetColdPenetration());
	ChaosPenetrationChangedDelegate.Execute(AttributesComponent->GetChaosPenetration());

	/** Defence */
	ArmorChangedDelegate.Execute(AttributesComponent->GetArmor());
	EvasionChangedDelegate.Execute(AttributesComponent->GetEvasion());
	HealthRegenerationChangedDelegate.Execute(AttributesComponent->GetHealthRegeneration());
	EnergyShieldRegenerationChangedDelegate.Execute(AttributesComponent->GetEnergyShieldRegeneration());
	FireResistanceChangedDelegate.Execute(AttributesComponent->GetFireResistance(), AttributesComponent->GetMaxFireResistance());
	ColdResistanceChangedDelegate.Execute(AttributesComponent->GetColdResistance(), AttributesComponent->GetMaxColdResistance());
	LightningResistanceChangedDelegate.Execute(AttributesComponent->GetLightningResistance(), AttributesComponent->GetMaxLightningResistance());
	ChaosResistanceChangedDelegate.Execute(AttributesComponent->GetChaosResistance(), AttributesComponent->GetMaxChaosResistance());
	SpellSuppressionChanceChangedDelegate.Execute(AttributesComponent->GetSpellSuppressionChance());
	SpellSuppressionMagnitudeChangedDelegate.Execute(AttributesComponent->GetSpellSuppressionMagnitude());
	HitBlockChanceChangedDelegate.Execute(AttributesComponent->GetHitBlockChance(), AttributesComponent->GetMaxHitBlockChance());
	SpellBlockChanceChangedDelegate.Execute(AttributesComponent->GetSpellBlockChance(), AttributesComponent->GetMaxSpellBlockChance());
}

void UOCharacterStatusWidgetController::HeroLevelUp(const int32 NewLevel)
{
	if(HeroLevelUpDelegate.IsBound())
	{
		HeroLevelUpDelegate.Broadcast(NewLevel);
	}
}

void UOCharacterStatusWidgetController::StrengthChanged(const FOnAttributeChangeData& Data) const
{
	if(StrengthValueChangedDelegate.IsBound())
	{
		StrengthValueChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::IntelligenceChanged(const FOnAttributeChangeData& Data) const
{
	if(IntelligenceValueChangedDelegate.IsBound())
	{
		IntelligenceValueChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::DexterityChanged(const FOnAttributeChangeData& Data) const
{
	if(DexterityValueChangedDelegate.IsBound())
	{
		DexterityValueChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::FaithChanged(const FOnAttributeChangeData& Data) const
{
	if(FaithValueChangedDelegate.IsBound())
	{
		FaithValueChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxHealthChangedDelegate.IsBound())
	{
		MaxHealthChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxManaChangedDelegate.IsBound())
	{
		MaxManaChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::MaxSpecialResourceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxSpecialResourceChangedDelegate.IsBound())
	{
		MaxSpecialResourceChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxEnergyShieldChangedDelegate.IsBound())
	{
		MaxEnergyShieldChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::ExperienceChanged(const FOnAttributeChangeData& Data) const
{
	if(ExperienceChangedDelegate.IsBound())
	{
		ExperienceChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::MaxExperienceChanged(const FOnAttributeChangeData& Data)
{
	MaxExperienceOldValue = Data.OldValue;
	
	if(MaxExperienceChangedDelegate.IsBound())
	{
		MaxExperienceChangedDelegate.Execute(Data.NewValue, MaxExperienceOldValue);
	}
}

void UOCharacterStatusWidgetController::AccuracyChanged(const FOnAttributeChangeData& Data) const
{
	if(AccuracyChangedDelegate.IsBound())
	{
		AccuracyChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::AttackSpeedChanged(const FOnAttributeChangeData& Data) const
{
	if(AttackSpeedChangedDelegate.IsBound())
	{
		AttackSpeedChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::CastSpeedChanged(const FOnAttributeChangeData& Data) const
{
	if(CastSpeedChangedDelegate.IsBound())
	{
		CastSpeedChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(CriticalStrikeChanceChangedDelegate.IsBound())
	{
		CriticalStrikeChanceChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::CriticalStrikeDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(CriticalStrikeDamageMultiplierChangedDelegate.IsBound())
	{
		CriticalStrikeDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::PhysicalDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(PhysicalDamageMultiplierChangedDelegate.IsBound())
	{
		PhysicalDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::FireDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(FireDamageMultiplierChangedDelegate.IsBound())
	{
		FireDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::LightningDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(LightningDamageMultiplierChangedDelegate.IsBound())
	{
		LightningDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::ColdDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(ColdDamageMultiplierChangedDelegate.IsBound())
	{
		ColdDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::ChaosDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(ChaosDamageMultiplierChangedDelegate.IsBound())
	{
		ChaosDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::FirePenetrationChanged(const FOnAttributeChangeData& Data) const
{
	if(FirePenetrationChangedDelegate.IsBound())
	{
		FirePenetrationChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::LightningPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	if(LightningPenetrationChangedDelegate.IsBound())
	{
		LightningPenetrationChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::ColdPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	if(ColdPenetrationChangedDelegate.IsBound())
	{
		ColdPenetrationChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::ChaosPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	if(ChaosPenetrationChangedDelegate.IsBound())
	{
		ChaosPenetrationChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::ArmorChanged(const FOnAttributeChangeData& Data) const
{
	if(ArmorChangedDelegate.IsBound())
	{
		ArmorChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::EvasionChanged(const FOnAttributeChangeData& Data) const
{
	if(EvasionChangedDelegate.IsBound())
	{
		EvasionChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::HealthRegenerationChanged(const FOnAttributeChangeData& Data) const
{
	if(HealthRegenerationChangedDelegate.IsBound())
	{
		HealthRegenerationChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::EnergyShieldRegenerationChanged(const FOnAttributeChangeData& Data) const
{
	if(EnergyShieldRegenerationChangedDelegate.IsBound())
	{
		EnergyShieldRegenerationChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::FireResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(FireResistanceChangedDelegate.IsBound())
	{
		FireResistanceChangedDelegate.Execute(Data.NewValue, AttributesComponent->GetMaxFireResistance());
	}
}

void UOCharacterStatusWidgetController::MaxFireResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxFireResistanceChangedDelegate.IsBound())
	{
		MaxFireResistanceChangedDelegate.Execute(AttributesComponent->GetFireResistance(), Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::ColdResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(ColdResistanceChangedDelegate.IsBound())
	{
		ColdResistanceChangedDelegate.Execute(Data.NewValue, AttributesComponent->GetMaxColdResistance());
	}
}

void UOCharacterStatusWidgetController::MaxColdResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxColdResistanceChangedDelegate.IsBound())
	{
		MaxColdResistanceChangedDelegate.Execute(AttributesComponent->GetColdResistance(), Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::LightningResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(LightningResistanceChangedDelegate.IsBound())
	{
		LightningResistanceChangedDelegate.Execute(Data.NewValue, AttributesComponent->GetMaxLightningResistance());
	}
}

void UOCharacterStatusWidgetController::MaxLightningResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxLightningResistanceChangedDelegate.IsBound())
	{
		MaxLightningResistanceChangedDelegate.Execute(AttributesComponent->GetLightningResistance(), Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::ChaosResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(ChaosResistanceChangedDelegate.IsBound())
	{
		ChaosResistanceChangedDelegate.Execute(Data.NewValue, AttributesComponent->GetMaxChaosResistance());
	}
}

void UOCharacterStatusWidgetController::MaxChaosResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxChaosResistanceChangedDelegate.IsBound())
	{
		MaxChaosResistanceChangedDelegate.Execute(AttributesComponent->GetChaosResistance(), Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::SpellSuppressionChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(SpellSuppressionChanceChangedDelegate.IsBound())
	{
		SpellSuppressionChanceChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::SpellSuppressionMagnitudeChanged(const FOnAttributeChangeData& Data) const
{
	if(SpellSuppressionMagnitudeChangedDelegate.IsBound())
	{
		SpellSuppressionMagnitudeChangedDelegate.Execute(Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::HitBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(HitBlockChanceChangedDelegate.IsBound())
	{
		HitBlockChanceChangedDelegate.Execute(Data.NewValue, AttributesComponent->GetMaxHitBlockChance());
	}
}

void UOCharacterStatusWidgetController::MaxHitBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxHitBlockChanceChangedDelegate.IsBound())
	{
		MaxHitBlockChanceChangedDelegate.Execute(AttributesComponent->GetHitBlockChance(), Data.NewValue);
	}
}

void UOCharacterStatusWidgetController::SpellBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(SpellBlockChanceChangedDelegate.IsBound())
	{
		SpellBlockChanceChangedDelegate.Execute(Data.NewValue, AttributesComponent->GetMaxSpellBlockChance());
	}
}

void UOCharacterStatusWidgetController::MaxSpellBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxSpellBlockChanceChangedDelegate.IsBound())
	{
		MaxSpellBlockChanceChangedDelegate.Execute(AttributesComponent->GetSpellBlockChance(), Data.NewValue);
	}
}



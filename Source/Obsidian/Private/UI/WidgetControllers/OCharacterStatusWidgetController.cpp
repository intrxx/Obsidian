// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/OCharacterStatusWidgetController.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"

void UOCharacterStatusWidgetController::OnWidgetControllerSetupCompleted()
{
	UObsidianAbilitySystemComponent* ObsidianASC = Cast<UObsidianAbilitySystemComponent>(AbilitySystemComponent);
	check(ObsidianASC);
	check(AttributesComponent);

	HandleBindingCallbacks(ObsidianASC);
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
	AllElementalDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetAllElementalDamageMultiplierAttribute()).AddUObject(this, &ThisClass::AllElementalDamageMultiplierChanged);
	ChaosDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetChaosDamageMultiplierAttribute()).AddUObject(this, &ThisClass::ChaosDamageMultiplierChanged);
	FirePenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetFirePenetrationAttribute()).AddUObject(this, &ThisClass::FirePenetrationChanged);
	LightningPenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetLightningPenetrationAttribute()).AddUObject(this, &ThisClass::LightningPenetrationChanged);
	ColdPenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetColdPenetrationAttribute()).AddUObject(this, &ThisClass::ColdPenetrationChanged);
	AllElementalPenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetAllElementalPenetrationAttribute()).AddUObject(this, &ThisClass::AllElementalPenetrationChanged);
	
	/** Defence */
	ArmorChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetArmorAttribute()).AddUObject(this, &ThisClass::ArmorChanged);
	EvasionChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetEvasionAttribute()).AddUObject(this, &ThisClass::EvasionChanged);
	HealthRegenerationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetHealthRegenerationAttribute()).AddUObject(this, &ThisClass::HealthRegenerationChanged);
	EnergyShieldRegenerationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetEnergyShieldRegenerationAttribute()).AddUObject(this, &ThisClass::EnergyShieldRegenerationChanged);
	AllElementalResistancesChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetAllElementalResistancesAttribute()).AddUObject(this, &ThisClass::AllElementalResistancesChanged);
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
	ExperienceChangedDelegate.Execute(AttributesComponent->GetExperience());
	MaxExperienceChangedDelegate.Execute(AttributesComponent->GetMaxExperience());
	
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
	AllElementalDamageMultiplierChangedDelegate.Execute(AttributesComponent->GetAllElementalDamageMultiplier());
	ChaosDamageMultiplierChangedDelegate.Execute(AttributesComponent->GetChaosDamageMultiplier());
	FirePenetrationChangedDelegate.Execute(AttributesComponent->GetFirePenetration());
	LightningPenetrationChangedDelegate.Execute(AttributesComponent->GetLightningPenetration());
	ColdPenetrationChangedDelegate.Execute(AttributesComponent->GetColdPenetration());
	AllElementalPenetrationChangedDelegate.Execute(AttributesComponent->GetAllElementalPenetration());

	/** Defence */
	ArmorChangedDelegate.Execute(AttributesComponent->GetArmor());
	EvasionChangedDelegate.Execute(AttributesComponent->GetEvasion());
	HealthRegenerationChangedDelegate.Execute(AttributesComponent->GetHealthRegeneration());
	EnergyShieldRegenerationChangedDelegate.Execute(AttributesComponent->GetEnergyShieldRegeneration());
	FireResistanceChangedDelegate.Execute(AttributesComponent->GetFireResistance(), AttributesComponent->GetMaxFireResistance());
	ColdResistanceChangedDelegate.Execute(AttributesComponent->GetColdResistance(), AttributesComponent->GetMaxColdResistance());
	LightningResistanceChangedDelegate.Execute(AttributesComponent->GetLightningResistance(), AttributesComponent->GetMaxLightningResistance());
	ChaosResistanceChangedDelegate.Execute(AttributesComponent->GetChaosResistance(), AttributesComponent->GetMaxChaosResistance());
	AllElementalResistancesChangedDelegate.Execute(AttributesComponent->GetAllElementalResistances());
	SpellSuppressionChanceChangedDelegate.Execute(AttributesComponent->GetSpellSuppressionChance());
	SpellSuppressionMagnitudeChangedDelegate.Execute(AttributesComponent->GetSpellSuppressionMagnitude());
	HitBlockChanceChangedDelegate.Execute(AttributesComponent->GetHitBlockChance(), AttributesComponent->GetMaxHitBlockChance());
	SpellBlockChanceChangedDelegate.Execute(AttributesComponent->GetSpellBlockChance(), AttributesComponent->GetMaxSpellBlockChance());
}

void UOCharacterStatusWidgetController::StrengthChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	StrengthValueChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::IntelligenceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	IntelligenceValueChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::DexterityChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	DexterityValueChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::FaithChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	FaithValueChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	MaxHealthChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	MaxManaChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::MaxSpecialResourceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	MaxSpecialResourceChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	MaxEnergyShieldChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::ExperienceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	ExperienceChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::MaxExperienceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	MaxExperienceChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::AccuracyChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	AccuracyChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::AttackSpeedChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	AttackSpeedChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::CastSpeedChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	CastSpeedChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	CriticalStrikeChanceChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::CriticalStrikeDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	CriticalStrikeDamageMultiplierChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::PhysicalDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	PhysicalDamageMultiplierChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::FireDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	FireDamageMultiplierChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::LightningDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	LightningDamageMultiplierChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::ColdDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	ColdDamageMultiplierChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::AllElementalDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	AllElementalDamageMultiplierChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::ChaosDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	ChaosDamageMultiplierChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::FirePenetrationChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	FirePenetrationChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::LightningPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	LightningPenetrationChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::ColdPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	ColdPenetrationChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::AllElementalPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	AllElementalPenetrationChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::ArmorChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	ArmorChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::EvasionChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	EvasionChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::HealthRegenerationChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	HealthRegenerationChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::EnergyShieldRegenerationChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	EnergyShieldRegenerationChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::AllElementalResistancesChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	AllElementalResistancesChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::FireResistanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	FireResistanceChangedDelegate.ExecuteIfBound(NewValue, AttributesComponent->GetMaxFireResistance());
}

void UOCharacterStatusWidgetController::MaxFireResistanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	MaxFireResistanceChangedDelegate.ExecuteIfBound(AttributesComponent->GetFireResistance(), NewValue);
}

void UOCharacterStatusWidgetController::ColdResistanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	ColdResistanceChangedDelegate.ExecuteIfBound(NewValue, AttributesComponent->GetMaxColdResistance());
}

void UOCharacterStatusWidgetController::MaxColdResistanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	MaxColdResistanceChangedDelegate.ExecuteIfBound(AttributesComponent->GetColdResistance(), NewValue);
}

void UOCharacterStatusWidgetController::LightningResistanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	LightningResistanceChangedDelegate.ExecuteIfBound(NewValue, AttributesComponent->GetMaxLightningResistance());
}

void UOCharacterStatusWidgetController::MaxLightningResistanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	MaxLightningResistanceChangedDelegate.ExecuteIfBound(AttributesComponent->GetLightningResistance(), NewValue);
}

void UOCharacterStatusWidgetController::ChaosResistanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	ChaosResistanceChangedDelegate.ExecuteIfBound(NewValue, AttributesComponent->GetMaxChaosResistance());
}

void UOCharacterStatusWidgetController::MaxChaosResistanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;
	
	MaxChaosResistanceChangedDelegate.ExecuteIfBound(AttributesComponent->GetChaosResistance(), NewValue);
}

void UOCharacterStatusWidgetController::SpellSuppressionChanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	SpellSuppressionChanceChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::SpellSuppressionMagnitudeChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	SpellSuppressionMagnitudeChangedDelegate.ExecuteIfBound(NewValue);
}

void UOCharacterStatusWidgetController::HitBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	HitBlockChanceChangedDelegate.ExecuteIfBound(NewValue, AttributesComponent->GetMaxHitBlockChance());
}

void UOCharacterStatusWidgetController::MaxHitBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	MaxHitBlockChanceChangedDelegate.ExecuteIfBound(AttributesComponent->GetHitBlockChance(), NewValue);
}

void UOCharacterStatusWidgetController::SpellBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	SpellBlockChanceChangedDelegate.ExecuteIfBound(NewValue, AttributesComponent->GetMaxSpellBlockChance());
}

void UOCharacterStatusWidgetController::MaxSpellBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	const float NewValue = Data.NewValue;

	MaxSpellBlockChanceChangedDelegate.ExecuteIfBound(AttributesComponent->GetSpellBlockChance(), NewValue);
}



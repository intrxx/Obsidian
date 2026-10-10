// Copyright 2026 out of sCope team - intrxx

#include "UI/WidgetControllers/ObCharacterStatusWidgetController.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Obsidian/ObsidianLogCategories.h"


void UObCharacterStatusWidgetController::OnWidgetControllerSetupCompleted()
{
	check(OwnerPlayerController.IsValid());
	const AObsidianPlayerController* PlayerController = OwnerPlayerController.Get();
	if (PlayerController == nullptr)
	{
		UE_LOG(ObLogUICharacterStatus, Error, TEXT("PlayerController is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}

	if(const AObsidianHero* Hero = Cast<AObsidianHero>(PlayerController->GetCharacter()))
	{
		HeroClassText = UObsidianGameplayStatics::GetHeroClassText(Hero->GetHeroClass());
	}

	OwnerAttributesComponent = UObsidianHeroAttributesComponent::FindHeroAttributesComponent(
					PlayerController->GetPawn());
	check(OwnerAttributesComponent.IsValid());

	OwnerAbilitySystemComponent = PlayerController->GetObsidianAbilitySystemComponent();
	check(OwnerAbilitySystemComponent.IsValid());
	if (UObsidianAbilitySystemComponent* ObsidianASC = OwnerAbilitySystemComponent.Get())
	{
		HandleBindingCallbacks(ObsidianASC);
	}
	
	check(OwnerPlayerState.IsValid());
	if (AObsidianPlayerState* PlayerState = OwnerPlayerState.Get())
	{
		PlayerState->OnHeroLevelUp.AddDynamic(this, &ThisClass::HeroLevelUp);
		HeroNameString = PlayerState->GetObsidianPlayerName();
	}
}

void UObCharacterStatusWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* InObsidianASC)
{
	if(InObsidianASC == nullptr)
	{
		UE_LOG(ObLogUICharacterStatus, Error, TEXT("ObsidianASC is invalid in [%hs]."), __FUNCTION__);
		return;
	}

	const UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		UE_LOG(ObLogUICharacterStatus, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}
	
	/** Character */
	ExperienceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetExperienceAttribute()).AddUObject(this, &ThisClass::ExperienceChanged);
	MaxExperienceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxExperienceAttribute()).AddUObject(this, &ThisClass::MaxExperienceChanged);
	
	/** Attributes */
	StrengthChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetStrengthAttribute()).AddUObject(this, &ThisClass::StrengthChanged);
	IntelligenceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetIntelligenceAttribute()).AddUObject(this, &ThisClass::IntelligenceChanged);
	DexterityChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetDexterityAttribute()).AddUObject(this, &ThisClass::DexterityChanged);
	FaithChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetFaithAttribute()).AddUObject(this, &ThisClass::FaithChanged);
	
	/** Vital Attributes */
	MaxHealthChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	MaxEnergyShieldChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxEnergyShieldAttribute()).AddUObject(this, &ThisClass::MaxEnergyShieldChanged);
	MaxSpecialResourceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxSpecialResourceAttribute()).AddUObject(this, &ThisClass::MaxSpecialResourceChanged);
	MaxManaChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	MaxStaminaChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxStaminaAttribute()).AddUObject(this, &ThisClass::MaxStaminaChanged);
	StaminaRegenerationChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetStaminaRegenerationAttribute()).AddUObject(this, &ThisClass::StaminaRegenerationChanged);
	
	/** Offence */
	AccuracyChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetAccuracyAttribute()).AddUObject(this, &ThisClass::AccuracyChanged);
	AttackSpeedChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetAttackSpeedAttribute()).AddUObject(this, &ThisClass::AttackSpeedChanged);
	CastSpeedChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetCastSpeedAttribute()).AddUObject(this, &ThisClass::CastSpeedChanged);
	CriticalStrikeChanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetCriticalStrikeChanceAttribute()).AddUObject(this, &ThisClass::CriticalStrikeChanceChanged);
	CriticalStrikeDamageMultiplierChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetCriticalStrikeDamageMultiplierAttribute()).AddUObject(this, &ThisClass::CriticalStrikeDamageMultiplierChanged);
	PhysicalDamageMultiplierChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetPhysicalDamageMultiplierAttribute()).AddUObject(this, &ThisClass::PhysicalDamageMultiplierChanged);
	FireDamageMultiplierChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetFireDamageMultiplierAttribute()).AddUObject(this, &ThisClass::FireDamageMultiplierChanged);
	LightningDamageMultiplierChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetLightningDamageMultiplierAttribute()).AddUObject(this, &ThisClass::LightningDamageMultiplierChanged);
	ColdDamageMultiplierChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetColdDamageMultiplierAttribute()).AddUObject(this, &ThisClass::ColdDamageMultiplierChanged);
	ChaosDamageMultiplierChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetChaosDamageMultiplierAttribute()).AddUObject(this, &ThisClass::ChaosDamageMultiplierChanged);
	FirePenetrationChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetFirePenetrationAttribute()).AddUObject(this, &ThisClass::FirePenetrationChanged);
	LightningPenetrationChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetLightningPenetrationAttribute()).AddUObject(this, &ThisClass::LightningPenetrationChanged);
	ColdPenetrationChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetColdPenetrationAttribute()).AddUObject(this, &ThisClass::ColdPenetrationChanged);
	ChaosPenetrationChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetChaosPenetrationAttribute()).AddUObject(this, &ThisClass::ChaosPenetrationChanged);
	
	/** Defence */
	ArmorChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetArmorAttribute()).AddUObject(this, &ThisClass::ArmorChanged);
	EvasionChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetEvasionAttribute()).AddUObject(this, &ThisClass::EvasionChanged);
	HealthRegenerationChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetHealthRegenerationAttribute()).AddUObject(this, &ThisClass::HealthRegenerationChanged);
	EnergyShieldRegenerationChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetEnergyShieldRegenerationAttribute()).AddUObject(this, &ThisClass::EnergyShieldRegenerationChanged);
	FireResistanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetFireResistanceAttribute()).AddUObject(this, &ThisClass::FireResistanceChanged);
	MaxFireResistanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxFireResistanceAttribute()).AddUObject(this, &ThisClass::MaxFireResistanceChanged);
	ColdResistanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetColdResistanceAttribute()).AddUObject(this, &ThisClass::ColdResistanceChanged);
	MaxColdResistanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxColdResistanceAttribute()).AddUObject(this, &ThisClass::MaxColdResistanceChanged);
	LightningResistanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetLightningResistanceAttribute()).AddUObject(this, &ThisClass::LightningResistanceChanged);
	MaxLightningResistanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxLightningResistanceAttribute()).AddUObject(this, &ThisClass::MaxLightningResistanceChanged);
	ChaosResistanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetChaosResistanceAttribute()).AddUObject(this, &ThisClass::ChaosResistanceChanged);
	MaxChaosResistanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxChaosResistanceAttribute()).AddUObject(this, &ThisClass::MaxChaosResistanceChanged);
	SpellSuppressionChanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetSpellSuppressionChanceAttribute()).AddUObject(this, &ThisClass::SpellSuppressionChanceChanged);
	SpellSuppressionMagnitudeChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetSpellSuppressionMagnitudeAttribute()).AddUObject(this, &ThisClass::SpellSuppressionMagnitudeChanged);
	HitBlockChanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetHitBlockChanceAttribute()).AddUObject(this, &ThisClass::HitBlockChanceChanged);
	MaxHitBlockChanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxHitBlockChanceAttribute()).AddUObject(this, &ThisClass::MaxHitBlockChanceChanged);
	SpellBlockChanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetSpellBlockChanceAttribute()).AddUObject(this, &ThisClass::SpellBlockChanceChanged);
	MaxSpellBlockChanceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxSpellBlockChanceAttribute()).AddUObject(this, &ThisClass::MaxSpellBlockChanceChanged);
}

void UObCharacterStatusWidgetController::SetInitialAttributeValues() const
{
	AObsidianPlayerState* PlayerState = OwnerPlayerState.Get();
	if (PlayerState == nullptr)
	{
		UE_LOG(ObLogUICharacterStatus, Error, TEXT("PlayerState is invalid in [%hs]."),
				__FUNCTION__);
		return;
	}
	
	const UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		UE_LOG(ObLogUICharacterStatus, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}
	
	/** Character */
	HeroLevelUpDelegate.Broadcast(PlayerState->GetHeroLevel());
	ExperienceChangedDelegate.Execute(HeroAttributesComp->GetExperience());
	MaxExperienceChangedDelegate.Execute(HeroAttributesComp->GetMaxExperience(), MaxExperienceOldValue);
	
	/** Attributes */
	StrengthValueChangedDelegate.Execute(HeroAttributesComp->GetStrength());
	IntelligenceValueChangedDelegate.Execute(HeroAttributesComp->GetIntelligence());
	DexterityValueChangedDelegate.Execute(HeroAttributesComp->GetDexterity());
	FaithValueChangedDelegate.Execute(HeroAttributesComp->GetFaith());

	/** Vital Attributes */
	MaxHealthChangedDelegate.Execute(HeroAttributesComp->GetMaxHealth());
	MaxManaChangedDelegate.Execute(HeroAttributesComp->GetMaxMana());
	MaxSpecialResourceChangedDelegate.Execute(HeroAttributesComp->GetMaxSpecialResource());
	MaxEnergyShieldChangedDelegate.Execute(HeroAttributesComp->GetMaxEnergyShield());
	MaxStaminaChangedDelegate.Execute(HeroAttributesComp->GetMaxStamina());
	StaminaRegenerationChangedDelegate.Execute(HeroAttributesComp->GetStaminaRegeneration());
	
	/** Offence */
	AccuracyChangedDelegate.Execute(HeroAttributesComp->GetAccuracy());
	AttackSpeedChangedDelegate.Execute(HeroAttributesComp->GetAttackSpeed());
	CastSpeedChangedDelegate.Execute(HeroAttributesComp->GetCastSpeed());
	CriticalStrikeChanceChangedDelegate.Execute(HeroAttributesComp->GetCriticalStrikeChance());
	CriticalStrikeDamageMultiplierChangedDelegate.Execute(HeroAttributesComp->GetCriticalStrikeDamageMultiplier());
	PhysicalDamageMultiplierChangedDelegate.Execute(HeroAttributesComp->GetPhysicalDamageMultiplier());
	FireDamageMultiplierChangedDelegate.Execute(HeroAttributesComp->GetFireDamageMultiplier());
	LightningDamageMultiplierChangedDelegate.Execute(HeroAttributesComp->GetLightningDamageMultiplier());
	ColdDamageMultiplierChangedDelegate.Execute(HeroAttributesComp->GetColdDamageMultiplier());
	ChaosDamageMultiplierChangedDelegate.Execute(HeroAttributesComp->GetChaosDamageMultiplier());
	FirePenetrationChangedDelegate.Execute(HeroAttributesComp->GetFirePenetration());
	LightningPenetrationChangedDelegate.Execute(HeroAttributesComp->GetLightningPenetration());
	ColdPenetrationChangedDelegate.Execute(HeroAttributesComp->GetColdPenetration());
	ChaosPenetrationChangedDelegate.Execute(HeroAttributesComp->GetChaosPenetration());

	/** Defence */
	ArmorChangedDelegate.Execute(HeroAttributesComp->GetArmor());
	EvasionChangedDelegate.Execute(HeroAttributesComp->GetEvasion());
	HealthRegenerationChangedDelegate.Execute(HeroAttributesComp->GetHealthRegeneration());
	EnergyShieldRegenerationChangedDelegate.Execute(HeroAttributesComp->GetEnergyShieldRegeneration());
	FireResistanceChangedDelegate.Execute(HeroAttributesComp->GetFireResistance(), HeroAttributesComp->GetMaxFireResistance());
	ColdResistanceChangedDelegate.Execute(HeroAttributesComp->GetColdResistance(), HeroAttributesComp->GetMaxColdResistance());
	LightningResistanceChangedDelegate.Execute(HeroAttributesComp->GetLightningResistance(), HeroAttributesComp->GetMaxLightningResistance());
	ChaosResistanceChangedDelegate.Execute(HeroAttributesComp->GetChaosResistance(), HeroAttributesComp->GetMaxChaosResistance());
	SpellSuppressionChanceChangedDelegate.Execute(HeroAttributesComp->GetSpellSuppressionChance());
	SpellSuppressionMagnitudeChangedDelegate.Execute(HeroAttributesComp->GetSpellSuppressionMagnitude());
	HitBlockChanceChangedDelegate.Execute(HeroAttributesComp->GetHitBlockChance(), HeroAttributesComp->GetMaxHitBlockChance());
	SpellBlockChanceChangedDelegate.Execute(HeroAttributesComp->GetSpellBlockChance(), HeroAttributesComp->GetMaxSpellBlockChance());
}

void UObCharacterStatusWidgetController::HeroLevelUp(const uint8 InNewLevel)
{
	if(HeroLevelUpDelegate.IsBound())
	{
		HeroLevelUpDelegate.Broadcast(InNewLevel);
	}
}

void UObCharacterStatusWidgetController::StrengthChanged(const FOnAttributeChangeData& InData) const
{
	if(StrengthValueChangedDelegate.IsBound())
	{
		StrengthValueChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::IntelligenceChanged(const FOnAttributeChangeData& InData) const
{
	if(IntelligenceValueChangedDelegate.IsBound())
	{
		IntelligenceValueChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::DexterityChanged(const FOnAttributeChangeData& InData) const
{
	if(DexterityValueChangedDelegate.IsBound())
	{
		DexterityValueChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::FaithChanged(const FOnAttributeChangeData& InData) const
{
	if(FaithValueChangedDelegate.IsBound())
	{
		FaithValueChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxHealthChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxHealthChangedDelegate.IsBound())
	{
		MaxHealthChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxManaChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxManaChangedDelegate.IsBound())
	{
		MaxManaChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxSpecialResourceChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxSpecialResourceChangedDelegate.IsBound())
	{
		MaxSpecialResourceChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxEnergyShieldChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxEnergyShieldChangedDelegate.IsBound())
	{
		MaxEnergyShieldChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxStaminaChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxStaminaChangedDelegate.IsBound())
	{
		MaxStaminaChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::StaminaRegenerationChanged(const FOnAttributeChangeData& InData) const
{
	if(StaminaRegenerationChangedDelegate.IsBound())
	{
		StaminaRegenerationChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::ExperienceChanged(const FOnAttributeChangeData& InData) const
{
	if(ExperienceChangedDelegate.IsBound())
	{
		ExperienceChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxExperienceChanged(const FOnAttributeChangeData& InData)
{
	MaxExperienceOldValue = InData.OldValue;
	
	if(MaxExperienceChangedDelegate.IsBound())
	{
		MaxExperienceChangedDelegate.Execute(InData.NewValue, MaxExperienceOldValue);
	}
}

void UObCharacterStatusWidgetController::AccuracyChanged(const FOnAttributeChangeData& InData) const
{
	if(AccuracyChangedDelegate.IsBound())
	{
		AccuracyChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::AttackSpeedChanged(const FOnAttributeChangeData& InData) const
{
	if(AttackSpeedChangedDelegate.IsBound())
	{
		AttackSpeedChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::CastSpeedChanged(const FOnAttributeChangeData& InData) const
{
	if(CastSpeedChangedDelegate.IsBound())
	{
		CastSpeedChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::CriticalStrikeChanceChanged(const FOnAttributeChangeData& InData) const
{
	if(CriticalStrikeChanceChangedDelegate.IsBound())
	{
		CriticalStrikeChanceChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::CriticalStrikeDamageMultiplierChanged(const FOnAttributeChangeData& InData) const
{
	if(CriticalStrikeDamageMultiplierChangedDelegate.IsBound())
	{
		CriticalStrikeDamageMultiplierChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::PhysicalDamageMultiplierChanged(const FOnAttributeChangeData& InData) const
{
	if(PhysicalDamageMultiplierChangedDelegate.IsBound())
	{
		PhysicalDamageMultiplierChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::FireDamageMultiplierChanged(const FOnAttributeChangeData& InData) const
{
	if(FireDamageMultiplierChangedDelegate.IsBound())
	{
		FireDamageMultiplierChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::LightningDamageMultiplierChanged(const FOnAttributeChangeData& InData) const
{
	if(LightningDamageMultiplierChangedDelegate.IsBound())
	{
		LightningDamageMultiplierChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::ColdDamageMultiplierChanged(const FOnAttributeChangeData& InData) const
{
	if(ColdDamageMultiplierChangedDelegate.IsBound())
	{
		ColdDamageMultiplierChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::ChaosDamageMultiplierChanged(const FOnAttributeChangeData& InData) const
{
	if(ChaosDamageMultiplierChangedDelegate.IsBound())
	{
		ChaosDamageMultiplierChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::FirePenetrationChanged(const FOnAttributeChangeData& InData) const
{
	if(FirePenetrationChangedDelegate.IsBound())
	{
		FirePenetrationChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::LightningPenetrationChanged(const FOnAttributeChangeData& InData) const
{
	if(LightningPenetrationChangedDelegate.IsBound())
	{
		LightningPenetrationChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::ColdPenetrationChanged(const FOnAttributeChangeData& InData) const
{
	if(ColdPenetrationChangedDelegate.IsBound())
	{
		ColdPenetrationChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::ChaosPenetrationChanged(const FOnAttributeChangeData& InData) const
{
	if(ChaosPenetrationChangedDelegate.IsBound())
	{
		ChaosPenetrationChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::ArmorChanged(const FOnAttributeChangeData& InData) const
{
	if(ArmorChangedDelegate.IsBound())
	{
		ArmorChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::EvasionChanged(const FOnAttributeChangeData& InData) const
{
	if(EvasionChangedDelegate.IsBound())
	{
		EvasionChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::HealthRegenerationChanged(const FOnAttributeChangeData& InData) const
{
	if(HealthRegenerationChangedDelegate.IsBound())
	{
		HealthRegenerationChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::EnergyShieldRegenerationChanged(const FOnAttributeChangeData& InData) const
{
	if(EnergyShieldRegenerationChangedDelegate.IsBound())
	{
		EnergyShieldRegenerationChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::FireResistanceChanged(const FOnAttributeChangeData& InData) const
{
	if(FireResistanceChangedDelegate.IsBound())
	{
		FireResistanceChangedDelegate.Execute(InData.NewValue, OwnerAttributesComponent->GetMaxFireResistance());
	}
}

void UObCharacterStatusWidgetController::MaxFireResistanceChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxFireResistanceChangedDelegate.IsBound())
	{
		MaxFireResistanceChangedDelegate.Execute(OwnerAttributesComponent->GetFireResistance(), InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::ColdResistanceChanged(const FOnAttributeChangeData& InData) const
{
	if(ColdResistanceChangedDelegate.IsBound())
	{
		ColdResistanceChangedDelegate.Execute(InData.NewValue, OwnerAttributesComponent->GetMaxColdResistance());
	}
}

void UObCharacterStatusWidgetController::MaxColdResistanceChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxColdResistanceChangedDelegate.IsBound())
	{
		MaxColdResistanceChangedDelegate.Execute(OwnerAttributesComponent->GetColdResistance(), InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::LightningResistanceChanged(const FOnAttributeChangeData& InData) const
{
	if(LightningResistanceChangedDelegate.IsBound())
	{
		LightningResistanceChangedDelegate.Execute(InData.NewValue, OwnerAttributesComponent->GetMaxLightningResistance());
	}
}

void UObCharacterStatusWidgetController::MaxLightningResistanceChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxLightningResistanceChangedDelegate.IsBound())
	{
		MaxLightningResistanceChangedDelegate.Execute(OwnerAttributesComponent->GetLightningResistance(), InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::ChaosResistanceChanged(const FOnAttributeChangeData& InData) const
{
	if(ChaosResistanceChangedDelegate.IsBound())
	{
		ChaosResistanceChangedDelegate.Execute(InData.NewValue, OwnerAttributesComponent->GetMaxChaosResistance());
	}
}

void UObCharacterStatusWidgetController::MaxChaosResistanceChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxChaosResistanceChangedDelegate.IsBound())
	{
		MaxChaosResistanceChangedDelegate.Execute(OwnerAttributesComponent->GetChaosResistance(), InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::SpellSuppressionChanceChanged(const FOnAttributeChangeData& InData) const
{
	if(SpellSuppressionChanceChangedDelegate.IsBound())
	{
		SpellSuppressionChanceChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::SpellSuppressionMagnitudeChanged(const FOnAttributeChangeData& InData) const
{
	if(SpellSuppressionMagnitudeChangedDelegate.IsBound())
	{
		SpellSuppressionMagnitudeChangedDelegate.Execute(InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::HitBlockChanceChanged(const FOnAttributeChangeData& InData) const
{
	if(HitBlockChanceChangedDelegate.IsBound())
	{
		HitBlockChanceChangedDelegate.Execute(InData.NewValue, OwnerAttributesComponent->GetMaxHitBlockChance());
	}
}

void UObCharacterStatusWidgetController::MaxHitBlockChanceChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxHitBlockChanceChangedDelegate.IsBound())
	{
		MaxHitBlockChanceChangedDelegate.Execute(OwnerAttributesComponent->GetHitBlockChance(), InData.NewValue);
	}
}

void UObCharacterStatusWidgetController::SpellBlockChanceChanged(const FOnAttributeChangeData& InData) const
{
	if(SpellBlockChanceChangedDelegate.IsBound())
	{
		SpellBlockChanceChangedDelegate.Execute(InData.NewValue, OwnerAttributesComponent->GetMaxSpellBlockChance());
	}
}

void UObCharacterStatusWidgetController::MaxSpellBlockChanceChanged(const FOnAttributeChangeData& InData) const
{
	if(MaxSpellBlockChanceChangedDelegate.IsBound())
	{
		MaxSpellBlockChanceChangedDelegate.Execute(OwnerAttributesComponent->GetSpellBlockChance(), InData.NewValue);
	}
}



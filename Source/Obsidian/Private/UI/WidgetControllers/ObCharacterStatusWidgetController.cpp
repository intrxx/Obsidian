// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/WidgetControllers/ObCharacterStatusWidgetController.h"


#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Core/ObsidianGameplayStatics.h"

DEFINE_LOG_CATEGORY(LogWidgetController_CharacterStatus)

void UObCharacterStatusWidgetController::OnWidgetControllerSetupCompleted()
{
	check(OwnerPlayerController.IsValid());
	const AObsidianPlayerController* PlayerController = OwnerPlayerController.Get();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogWidgetController_CharacterStatus, Error, TEXT("PlayerController is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}

	if(const AObsidianHero* Hero = Cast<AObsidianHero>(PlayerController->GetCharacter()))
	{
		HeroClassText = UObsidianGameplayStatics::GetHeroClassText(Hero->GetHeroClass());
	}

	OwnerAttributesComponent = UObsidianHeroAttributesComponent::FindAttributesComponent(
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

void UObCharacterStatusWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
	if(ObsidianASC == nullptr)
	{
		UE_LOG(LogWidgetController_CharacterStatus, Error, TEXT("ObsidianASC is invalid in [%hs]."), __FUNCTION__);
		return;
	}

	const UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		UE_LOG(LogWidgetController_CharacterStatus, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}
	
	/** Character */
	ExperienceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetExperienceAttribute()).AddUObject(this, &ThisClass::ExperienceChanged);
	MaxExperienceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxExperienceAttribute()).AddUObject(this, &ThisClass::MaxExperienceChanged);
	
	/** Attributes */
	StrengthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetStrengthAttribute()).AddUObject(this, &ThisClass::StrengthChanged);
	IntelligenceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetIntelligenceAttribute()).AddUObject(this, &ThisClass::IntelligenceChanged);
	DexterityChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetDexterityAttribute()).AddUObject(this, &ThisClass::DexterityChanged);
	FaithChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetFaithAttribute()).AddUObject(this, &ThisClass::FaithChanged);
	
	/** Vital Attributes */
	MaxHealthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	MaxEnergyShieldChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxEnergyShieldAttribute()).AddUObject(this, &ThisClass::MaxEnergyShieldChanged);
	MaxSpecialResourceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxSpecialResourceAttribute()).AddUObject(this, &ThisClass::MaxSpecialResourceChanged);
	MaxManaChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	
	/** Offence */
	AccuracyChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetAccuracyAttribute()).AddUObject(this, &ThisClass::AccuracyChanged);
	AttackSpeedChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetAttackSpeedAttribute()).AddUObject(this, &ThisClass::AttackSpeedChanged);
	CastSpeedChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetCastSpeedAttribute()).AddUObject(this, &ThisClass::CastSpeedChanged);
	CriticalStrikeChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetCriticalStrikeChanceAttribute()).AddUObject(this, &ThisClass::CriticalStrikeChanceChanged);
	CriticalStrikeDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetCriticalStrikeDamageMultiplierAttribute()).AddUObject(this, &ThisClass::CriticalStrikeDamageMultiplierChanged);
	PhysicalDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetPhysicalDamageMultiplierAttribute()).AddUObject(this, &ThisClass::PhysicalDamageMultiplierChanged);
	FireDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetFireDamageMultiplierAttribute()).AddUObject(this, &ThisClass::FireDamageMultiplierChanged);
	LightningDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetLightningDamageMultiplierAttribute()).AddUObject(this, &ThisClass::LightningDamageMultiplierChanged);
	ColdDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetColdDamageMultiplierAttribute()).AddUObject(this, &ThisClass::ColdDamageMultiplierChanged);
	ChaosDamageMultiplierChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetChaosDamageMultiplierAttribute()).AddUObject(this, &ThisClass::ChaosDamageMultiplierChanged);
	FirePenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetFirePenetrationAttribute()).AddUObject(this, &ThisClass::FirePenetrationChanged);
	LightningPenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetLightningPenetrationAttribute()).AddUObject(this, &ThisClass::LightningPenetrationChanged);
	ColdPenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetColdPenetrationAttribute()).AddUObject(this, &ThisClass::ColdPenetrationChanged);
	ChaosPenetrationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetChaosPenetrationAttribute()).AddUObject(this, &ThisClass::ChaosPenetrationChanged);
	
	/** Defence */
	ArmorChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetArmorAttribute()).AddUObject(this, &ThisClass::ArmorChanged);
	EvasionChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetEvasionAttribute()).AddUObject(this, &ThisClass::EvasionChanged);
	HealthRegenerationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetHealthRegenerationAttribute()).AddUObject(this, &ThisClass::HealthRegenerationChanged);
	EnergyShieldRegenerationChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetEnergyShieldRegenerationAttribute()).AddUObject(this, &ThisClass::EnergyShieldRegenerationChanged);
	FireResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetFireResistanceAttribute()).AddUObject(this, &ThisClass::FireResistanceChanged);
	MaxFireResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxFireResistanceAttribute()).AddUObject(this, &ThisClass::MaxFireResistanceChanged);
	ColdResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetColdResistanceAttribute()).AddUObject(this, &ThisClass::ColdResistanceChanged);
	MaxColdResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxColdResistanceAttribute()).AddUObject(this, &ThisClass::MaxColdResistanceChanged);
	LightningResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetLightningResistanceAttribute()).AddUObject(this, &ThisClass::LightningResistanceChanged);
	MaxLightningResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxLightningResistanceAttribute()).AddUObject(this, &ThisClass::MaxLightningResistanceChanged);
	ChaosResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetChaosResistanceAttribute()).AddUObject(this, &ThisClass::ChaosResistanceChanged);
	MaxChaosResistanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxChaosResistanceAttribute()).AddUObject(this, &ThisClass::MaxChaosResistanceChanged);
	SpellSuppressionChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetSpellSuppressionChanceAttribute()).AddUObject(this, &ThisClass::SpellSuppressionChanceChanged);
	SpellSuppressionMagnitudeChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetSpellSuppressionMagnitudeAttribute()).AddUObject(this, &ThisClass::SpellSuppressionMagnitudeChanged);
	HitBlockChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetHitBlockChanceAttribute()).AddUObject(this, &ThisClass::HitBlockChanceChanged);
	MaxHitBlockChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxHitBlockChanceAttribute()).AddUObject(this, &ThisClass::MaxHitBlockChanceChanged);
	SpellBlockChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetSpellBlockChanceAttribute()).AddUObject(this, &ThisClass::SpellBlockChanceChanged);
	MaxSpellBlockChanceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxSpellBlockChanceAttribute()).AddUObject(this, &ThisClass::MaxSpellBlockChanceChanged);
}

void UObCharacterStatusWidgetController::SetInitialAttributeValues() const
{
	AObsidianPlayerState* PlayerState = OwnerPlayerState.Get();
	if (PlayerState == nullptr)
	{
		UE_LOG(LogWidgetController_CharacterStatus, Error, TEXT("PlayerState is invalid in [%hs]."),
				__FUNCTION__);
		return;
	}
	
	const UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		UE_LOG(LogWidgetController_CharacterStatus, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
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

void UObCharacterStatusWidgetController::HeroLevelUp(const uint8 NewLevel)
{
	if(HeroLevelUpDelegate.IsBound())
	{
		HeroLevelUpDelegate.Broadcast(NewLevel);
	}
}

void UObCharacterStatusWidgetController::StrengthChanged(const FOnAttributeChangeData& Data) const
{
	if(StrengthValueChangedDelegate.IsBound())
	{
		StrengthValueChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::IntelligenceChanged(const FOnAttributeChangeData& Data) const
{
	if(IntelligenceValueChangedDelegate.IsBound())
	{
		IntelligenceValueChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::DexterityChanged(const FOnAttributeChangeData& Data) const
{
	if(DexterityValueChangedDelegate.IsBound())
	{
		DexterityValueChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::FaithChanged(const FOnAttributeChangeData& Data) const
{
	if(FaithValueChangedDelegate.IsBound())
	{
		FaithValueChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxHealthChangedDelegate.IsBound())
	{
		MaxHealthChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxManaChangedDelegate.IsBound())
	{
		MaxManaChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxSpecialResourceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxSpecialResourceChangedDelegate.IsBound())
	{
		MaxSpecialResourceChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxEnergyShieldChangedDelegate.IsBound())
	{
		MaxEnergyShieldChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::ExperienceChanged(const FOnAttributeChangeData& Data) const
{
	if(ExperienceChangedDelegate.IsBound())
	{
		ExperienceChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::MaxExperienceChanged(const FOnAttributeChangeData& Data)
{
	MaxExperienceOldValue = Data.OldValue;
	
	if(MaxExperienceChangedDelegate.IsBound())
	{
		MaxExperienceChangedDelegate.Execute(Data.NewValue, MaxExperienceOldValue);
	}
}

void UObCharacterStatusWidgetController::AccuracyChanged(const FOnAttributeChangeData& Data) const
{
	if(AccuracyChangedDelegate.IsBound())
	{
		AccuracyChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::AttackSpeedChanged(const FOnAttributeChangeData& Data) const
{
	if(AttackSpeedChangedDelegate.IsBound())
	{
		AttackSpeedChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::CastSpeedChanged(const FOnAttributeChangeData& Data) const
{
	if(CastSpeedChangedDelegate.IsBound())
	{
		CastSpeedChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(CriticalStrikeChanceChangedDelegate.IsBound())
	{
		CriticalStrikeChanceChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::CriticalStrikeDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(CriticalStrikeDamageMultiplierChangedDelegate.IsBound())
	{
		CriticalStrikeDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::PhysicalDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(PhysicalDamageMultiplierChangedDelegate.IsBound())
	{
		PhysicalDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::FireDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(FireDamageMultiplierChangedDelegate.IsBound())
	{
		FireDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::LightningDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(LightningDamageMultiplierChangedDelegate.IsBound())
	{
		LightningDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::ColdDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(ColdDamageMultiplierChangedDelegate.IsBound())
	{
		ColdDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::ChaosDamageMultiplierChanged(const FOnAttributeChangeData& Data) const
{
	if(ChaosDamageMultiplierChangedDelegate.IsBound())
	{
		ChaosDamageMultiplierChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::FirePenetrationChanged(const FOnAttributeChangeData& Data) const
{
	if(FirePenetrationChangedDelegate.IsBound())
	{
		FirePenetrationChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::LightningPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	if(LightningPenetrationChangedDelegate.IsBound())
	{
		LightningPenetrationChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::ColdPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	if(ColdPenetrationChangedDelegate.IsBound())
	{
		ColdPenetrationChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::ChaosPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	if(ChaosPenetrationChangedDelegate.IsBound())
	{
		ChaosPenetrationChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::ArmorChanged(const FOnAttributeChangeData& Data) const
{
	if(ArmorChangedDelegate.IsBound())
	{
		ArmorChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::EvasionChanged(const FOnAttributeChangeData& Data) const
{
	if(EvasionChangedDelegate.IsBound())
	{
		EvasionChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::HealthRegenerationChanged(const FOnAttributeChangeData& Data) const
{
	if(HealthRegenerationChangedDelegate.IsBound())
	{
		HealthRegenerationChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::EnergyShieldRegenerationChanged(const FOnAttributeChangeData& Data) const
{
	if(EnergyShieldRegenerationChangedDelegate.IsBound())
	{
		EnergyShieldRegenerationChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::FireResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(FireResistanceChangedDelegate.IsBound())
	{
		FireResistanceChangedDelegate.Execute(Data.NewValue, OwnerAttributesComponent->GetMaxFireResistance());
	}
}

void UObCharacterStatusWidgetController::MaxFireResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxFireResistanceChangedDelegate.IsBound())
	{
		MaxFireResistanceChangedDelegate.Execute(OwnerAttributesComponent->GetFireResistance(), Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::ColdResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(ColdResistanceChangedDelegate.IsBound())
	{
		ColdResistanceChangedDelegate.Execute(Data.NewValue, OwnerAttributesComponent->GetMaxColdResistance());
	}
}

void UObCharacterStatusWidgetController::MaxColdResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxColdResistanceChangedDelegate.IsBound())
	{
		MaxColdResistanceChangedDelegate.Execute(OwnerAttributesComponent->GetColdResistance(), Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::LightningResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(LightningResistanceChangedDelegate.IsBound())
	{
		LightningResistanceChangedDelegate.Execute(Data.NewValue, OwnerAttributesComponent->GetMaxLightningResistance());
	}
}

void UObCharacterStatusWidgetController::MaxLightningResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxLightningResistanceChangedDelegate.IsBound())
	{
		MaxLightningResistanceChangedDelegate.Execute(OwnerAttributesComponent->GetLightningResistance(), Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::ChaosResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(ChaosResistanceChangedDelegate.IsBound())
	{
		ChaosResistanceChangedDelegate.Execute(Data.NewValue, OwnerAttributesComponent->GetMaxChaosResistance());
	}
}

void UObCharacterStatusWidgetController::MaxChaosResistanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxChaosResistanceChangedDelegate.IsBound())
	{
		MaxChaosResistanceChangedDelegate.Execute(OwnerAttributesComponent->GetChaosResistance(), Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::SpellSuppressionChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(SpellSuppressionChanceChangedDelegate.IsBound())
	{
		SpellSuppressionChanceChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::SpellSuppressionMagnitudeChanged(const FOnAttributeChangeData& Data) const
{
	if(SpellSuppressionMagnitudeChangedDelegate.IsBound())
	{
		SpellSuppressionMagnitudeChangedDelegate.Execute(Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::HitBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(HitBlockChanceChangedDelegate.IsBound())
	{
		HitBlockChanceChangedDelegate.Execute(Data.NewValue, OwnerAttributesComponent->GetMaxHitBlockChance());
	}
}

void UObCharacterStatusWidgetController::MaxHitBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxHitBlockChanceChangedDelegate.IsBound())
	{
		MaxHitBlockChanceChangedDelegate.Execute(OwnerAttributesComponent->GetHitBlockChance(), Data.NewValue);
	}
}

void UObCharacterStatusWidgetController::SpellBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(SpellBlockChanceChangedDelegate.IsBound())
	{
		SpellBlockChanceChangedDelegate.Execute(Data.NewValue, OwnerAttributesComponent->GetMaxSpellBlockChance());
	}
}

void UObCharacterStatusWidgetController::MaxSpellBlockChanceChanged(const FOnAttributeChangeData& Data) const
{
	if(MaxSpellBlockChanceChangedDelegate.IsBound())
	{
		MaxSpellBlockChanceChangedDelegate.Execute(OwnerAttributesComponent->GetSpellBlockChance(), Data.NewValue);
	}
}



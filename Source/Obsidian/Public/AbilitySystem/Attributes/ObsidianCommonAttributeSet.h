// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianAttributeSetBase.h"
#include "ObsidianCommonAttributeSet.generated.h"

namespace ObsidianAttributeConstants
{
	constexpr float HitReactThreshold = (30.f/100.f) * 100.f;
}

/**
 * Attribute set that holds all the shared attributes between enemies and the player character
 */
UCLASS()
class OBSIDIAN_API UObsidianCommonAttributeSet : public UObsidianAttributeSetBase
{
	GENERATED_BODY()

public:
	UObsidianCommonAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/**
	 * Vital
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, EnergyShield);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, MaxEnergyShield);

	/**
	 * Status
	 */

	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, HealthRegeneration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, EnergyShieldRegeneration);

	/**
	 * Defence attributes
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, Evasion);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, SpellSuppressionChance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, SpellSuppressionMagnitude);
	
	/**
	 * Resistances
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, AllElementalResistances);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, FireResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, MaxFireResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ColdResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, MaxColdResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, LightningResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, MaxLightningResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ChaosResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, MaxChaosResistance);
	
	/**
	 * Damage scaling attributes
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, Accuracy);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, CriticalStrikeChance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, CriticalStrikeDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, AttackSpeed);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, CastSpeed);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, FirePenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ColdPenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, LightningPenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, AllElementalPenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ChaosPenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, FireDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ColdDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, LightningDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, AllElementalDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ChaosDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, PhysicalDamageMultiplier);

	/**
	 * Base Attributes
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, BaseDamage);
	
	/**
	 * Meta Attributes
	 */

	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, IncomingDamage);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, IncomingHealthHealing);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, IncomingEnergyShieldHealing);

	/**
	 * Character
	 */

	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, MovementSpeed);

public:
	/** Delegate fired when the health reaches zero */
	mutable FObsidianAttributeEvent OnOutOfHealth;
	
protected:
	/**
	 * Vital
	 */
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_EnergyShield(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxEnergyShield(const FGameplayAttributeData& OldValue);

	/**
	 * Status
	 */

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_EnergyShieldRegeneration(const FGameplayAttributeData& OldValue);

	/**
	 * Defence attributes
	 */
	
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Evasion(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SpellSuppressionChance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SpellSuppressionMagnitude(const FGameplayAttributeData& OldValue);

	/**
	 * Resistances
	 */
	
	UFUNCTION()
	void OnRep_AllElementalResistances(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxFireResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ColdResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxColdResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxLightningResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ChaosResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxChaosResistance(const FGameplayAttributeData& OldValue);

	/**
	 * Damage scaling attributes
	 */
	
	UFUNCTION()
	void OnRep_Accuracy(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_CriticalStrikeChance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_CriticalStrikeDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_CastSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_FirePenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ColdPenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_LightningPenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_AllElementalPenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ChaosPenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_FireDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ColdDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_LightningDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_AllElementalDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ChaosDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_PhysicalDamageMultiplier(const FGameplayAttributeData& OldValue);

	/**
	 * Base Attributes
	 */
	
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
	
	/**
	 * Base Attributes
	 */

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldValue);

protected:
private:
	bool bOutOfHealth;
	
	/**
	 * Vital
	 */
	
	/** The current Health attribute. The Health will be capped by the Max Health attribute. Health is hidden from modifiers so only Executions can modify it. */
	/** ---------------------------------------------------------------------------------------------------------- HideFromModifiers */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Obsidian|CAttributes|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	/** The current max Health attribute. Defines cap for health, Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Obsidian|CAttributes|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	/** The current Energy Shield attribute. The Energy Shield will be capped by the Max Energy Shield attribute. Energy Shield is hidden from modifiers so only Executions can modify it. */
	/** ---------------------------------------------------------------------------------------------------------------------- HideFromModifiers */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyShield, Category = "Obsidian|CAttributes|EnergyShield", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EnergyShield;

	/** The current Max Energy Shield attribute. Defines cap for Energy Shield, Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxEnergyShield, Category = "Obsidian|CAttributes|EnergyShield", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxEnergyShield;

	/**
	 * Status
	 */

	/** The current Health Regeneration attribute. Defines value of Health that is regenerated per second. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Obsidian|CAttributes|HealthRegeneration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthRegeneration;
	
	/** The current Energy Shield Regeneration attribute. Defines value of Energy Shield that is regenerated per second. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyShieldRegeneration, Category = "Obsidian|CAttributes|EnergyShieldRegeneration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EnergyShieldRegeneration;

	/**
	 * Defence attributes
	 */

	/** The current Armor attribute. Armor negates physical damage taken. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Obsidian|CAttributes|Armor", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Armor;

	/** The current Evasion attribute. Evasion grants a chance to evade attacks. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Evasion, Category = "Obsidian|CAttributes|Evasion", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Evasion;

	/** The current Armor Spell Suppression Chance Attribute [0% - 100%]. Chance to suppress Spell Damage. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpellSuppressionChance, Category = "Obsidian|CAttributes|SpellSuppressionChance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SpellSuppressionChance;

	/** The current Spell Suppression Magnitude [0% - 100%]. Percentage of Spell Damage to suppress (negate) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpellSuppressionMagnitude, Category = "Obsidian|CAttributes|SpellSuppressionMagnitude", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SpellSuppressionMagnitude;

	/**
	 * Resistances
	 */

	/**
	 * The current All Elemental Resistances attribute.
	 * 
	 * This is all elemental resistances put together for convenience (mainly scaling enemy's resistances) and I can have additional stat on items.
	 * This does not mean that this value holds all elemental resistances, but it is rather a way to scale all of them with one attribute.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AllElementalResistances, Category = "Obsidian|CAttributes|AllElementalResistances", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AllElementalResistances;
	
	/** The current Fire Resistance attribute. Fire Resistance negates fire damage taken. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Obsidian|CAttributes|FireResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FireResistance;

	/** The current Max Fire Resistance attribute. Defines cap for Fire Resistance. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxFireResistance, Category = "Obsidian|CAttributes|FireResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxFireResistance;

	/** The current Cold Resistance attribute. Cold Resistance negates cold damage taken. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ColdResistance, Category = "Obsidian|CAttributes|ColdResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ColdResistance;

	/** The current Max Cold Resistance attribute. Defines cap for Cold Resistance */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxColdResistance, Category = "Obsidian|CAttributes|ColdResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxColdResistance;

	/** The current Lightning Resistance attribute. Lightning Resistance negates lightning damage taken. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Obsidian|CAttributes|LightningResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LightningResistance;
	
	/** The current Max Lightning Resistance attribute. Defines cap for Lightning Resistance. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxLightningResistance, Category = "Obsidian|CAttributes|LightningResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxLightningResistance;
	
	/** The current Chaos Resistance attribute. Chaos resistance negates chaos damage taken. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChaosResistance, Category = "Obsidian|CAttributes|ChaosResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ChaosResistance;
	
	/** The current Max Chaos Resistance attribute. Defines cap for Chaos Resistance. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxChaosResistance, Category = "Obsidian|CAttributes|ChaosResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxChaosResistance;

	/**
	 * Damage related attributes
	 */

	/** The current Accuracy attribute. Source Accuracy combined with target Evasion determines a chance to hit */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Accuracy, Category = "Obsidian|CAttributes|Accuracy", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Accuracy;
	
	/**
	 * The current Critical Strike Chance attribute [0% - 100%]. Defines a chance to critical strike.
	 * We can critically strike with every instant damage.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalStrikeChance, Category = "Obsidian|CAttributes|CriticalStrikeChance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalStrikeChance;

	/** The current Critical Strike Multiplier attribute [x%]. While successfully critically striking we will treat the damage as Damage * x/100 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalStrikeDamageMultiplier, Category = "Obsidian|CAttributes|CriticalStrikeDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalStrikeDamageMultiplier;

	/** The current Attack Speed attribute. Attask speed increases the speed of attacks. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "Obsidian|CAttributes|AttackSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackSpeed;

	/** The current Cast Speed attribute. Cast Speed increases the speed of spells. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CastSpeed, Category = "Obsidian|CAttributes|CastSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CastSpeed;

	/** The current Fire Penetration attribute. Percentage of Fire Resistance that will be ignored. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FirePenetration, Category = "Obsidian|CAttributes|FirePenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FirePenetration;

	/** The current Cold Penetration attribute. Percentage of Cold Resistance that will be ignored. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ColdPenetration, Category = "Obsidian|CAttributes|ColdPenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ColdPenetration;

	/** The current Lightning Penetration attribute. Percentage of Lightning Resistance that will be ignored. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningPenetration, Category = "Obsidian|CAttributes|LightningPenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LightningPenetration;

	/** The current Elemental Penetration attribute. Percentage of all elemental resistances that will be ignored. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AllElementalPenetration, Category = "Obsidian|CAttributes|AllElementalPenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AllElementalPenetration;

	/** The current Chaos Penetration attribute. Percentage of Chaos Resistance that will be ignored. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChaosPenetration, Category = "Obsidian|CAttributes|ChaosPenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ChaosPenetration;
	
	/** The current Fire Damage Multiplier attribute. Multiplier for fire damage. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireDamageMultiplier, Category = "Obsidian|CAttributes|FireDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FireDamageMultiplier;

	/** The current Cold Damage Multiplier attribute. Multiplier for cold damage. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ColdDamageMultiplier, Category = "Obsidian|CAttributes|ColdDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ColdDamageMultiplier;

	/** The current Lightning Damage Multiplier attribute. Multiplier for lightning damage. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningDamageMultiplier, Category = "Obsidian|CAttributes|LightningDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LightningDamageMultiplier;

	/** The current All Elemental Damage Multiplier attribute. Multiplier for all elemental damage. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AllElementalDamageMultiplier, Category = "Obsidian|CAttributes|AllElementalDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AllElementalDamageMultiplier;

	/** The current Chaos Damage Multiplier attribute. Multiplier for chaos damage. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChaosDamageMultiplier, Category = "Obsidian|CAttributes|ChaosDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ChaosDamageMultiplier;

	/** The current Physical Damage Multiplier attribute. Multiplier for physical damage. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalDamageMultiplier, Category = "Obsidian|CAttributes|PhysicalDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData PhysicalDamageMultiplier;
	
	/**
	 * Base Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Obsidian|CAttributes|BaseDamage", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	/**
	 * Character
	 */

	/** The current Movement Speed attribute. Defines how fast the character is moving. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "Obsidian|CAttributes|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovementSpeed;

	/**
	 * Meta Attributes
	 */

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|CAttributes|Meta", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|CAttributes|Meta", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingHealthHealing;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|CAttributes|Meta", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingEnergyShieldHealing;
};


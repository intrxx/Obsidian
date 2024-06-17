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
	
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, CriticalStrikeChance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, CriticalStrikeMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, AttackSpeed);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, CastSpeed);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, FirePenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ColdPenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, LightningPenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ElementalPenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ChaosPenetration);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, FireDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ColdDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, LightningDamageMultiplier);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ElementalDamageMultiplier);
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
	void OnRep_CriticalStrikeChance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_CriticalStrikeMultiplier(const FGameplayAttributeData& OldValue);
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
	void OnRep_ElementalPenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ChaosPenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_FireDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ColdDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_LightningDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ElementalDamageMultiplier(const FGameplayAttributeData& OldValue);
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

private:
	bool bOutOfHealth;
	
	/**
	 * Vital
	 */
	
	/** The current Health attribute. The Health will be capped by the Max Health attribute. Health is hidden from modifiers so only Executions can modify it. */
	/** ---------------------------------------------------------------------------------------------------------- HideFromModifiers */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Obsidian|CAttributes|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	/** The current max Health attribute. Max Health is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Obsidian|CAttributes|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	/** The current Energy Shield attribute. The Energy Shield will be capped by the Max Energy Shield attribute. Energy Shield is hidden from modifiers so only Executions can modify it. */
	/** ---------------------------------------------------------------------------------------------------------------------- HideFromModifiers */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyShield, Category = "Obsidian|CAttributes|EnergyShield", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EnergyShield;

	/** The current Max Energy Shield attribute. MaxEnergy Shield is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxEnergyShield, Category = "Obsidian|CAttributes|MaxEnergyShield", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxEnergyShield;

	/**
	 * Status
	 */

	/** The current Health Regeneration attribute. Health Regeneration is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Obsidian|CAttributes|HealthRegeneration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthRegeneration;
	
	/** The current Energy Shield Regeneration attribute. Energy Shield Regeneration is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyShieldRegeneration, Category = "Obsidian|CAttributes|EnergyShieldRegeneration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EnergyShieldRegeneration;

	/**
	 * Defence attributes
	 */

	/** The current Armor attribute. Armor is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Obsidian|CAttributes|Armor", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Armor;

	/** The current Evasion attribute. Evasion is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Evasion, Category = "Obsidian|CAttributes|Evasion", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Evasion;

	/**
	 * Resistances
	 */

	/**
	 * The current All Elemental Resistances attribute. All Elemental Resistances is an attribute since Gameplay Effects can modify it.
	 * This is all elemental resistances put together for convenience (mainly scaling enemy's resistances) and we can have additional stat on items.
	 * 
	 * THIS DOES NOT MEAN THAT THIS VALUE HOLDS ALL ELEMENTAL RESISTANCES, BUT IT IS RATHER A WAY TO SCALE ALL OF THEM WITH ONE ATTRIBUTE.
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AllElementalResistances, Category = "Obsidian|CAttributes|AllElementalResistances", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AllElementalResistances;
	
	/** The current Fire Resistance attribute. Fire Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Obsidian|CAttributes|FireResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FireResistance;

	/** The current Max Fire Resistance attribute. Max Fire Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxFireResistance, Category = "Obsidian|CAttributes|MaxFireResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxFireResistance;

	/** The current Cold Resistance attribute. Cold Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ColdResistance, Category = "Obsidian|CAttributes|ColdResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ColdResistance;

	/** The current Max Cold Resistance attribute. Max Cold Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxColdResistance, Category = "Obsidian|CAttributes|MaxColdResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxColdResistance;

	/** The current Lightning Resistance attribute. Lightning Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Obsidian|CAttributes|LightningResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LightningResistance;
	
	/** The current Max Lightning Resistance attribute. Max Lightning Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxLightningResistance, Category = "Obsidian|CAttributes|MaxLightningResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxLightningResistance;
	
	/** The current Chaos Resistance attribute. Chaos Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChaosResistance, Category = "Obsidian|CAttributes|ChaosResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ChaosResistance;
	
	/** The current Max Chaos Resistance attribute. Max Chaos Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxChaosResistance, Category = "Obsidian|CAttributes|MaxChaosResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxChaosResistance;

	/**
	 * Damage scaling attributes
	 */

	/** The current Critical Strike Chance attribute [0, 100], treating it like a percentage. Critical Strike Chance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalStrikeChance, Category = "Obsidian|CAttributes|CriticalStrikeChance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalStrikeChance;

	/** The current Critical Strike Multiplier attribute. Critical Strike Multiplier is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalStrikeMultiplier, Category = "Obsidian|CAttributes|CriticalStrikeMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalStrikeMultiplier;

	/** The current Attack Speed attribute. Attack Speed is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "Obsidian|CAttributes|AttackSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackSpeed;

	/** The current Cast Speed attribute. Cast Speed is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CastSpeed, Category = "Obsidian|CAttributes|CastSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CastSpeed;

	/** The current Fire Penetration attribute. Fire Penetration is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FirePenetration, Category = "Obsidian|CAttributes|FirePenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FirePenetration;

	/** The current Cold Penetration attribute. Cold Penetration is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ColdPenetration, Category = "Obsidian|CAttributes|ColdPenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ColdPenetration;

	/** The current Lightning Penetration attribute. Lightning Penetration is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningPenetration, Category = "Obsidian|CAttributes|LightningPenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LightningPenetration;

	/** The current Elemental Penetration attribute. Elemental Penetration is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ElementalPenetration, Category = "Obsidian|CAttributes|ElementalPenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ElementalPenetration;

	/** The current Chaos Penetration attribute. Chaos Penetration is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChaosPenetration, Category = "Obsidian|CAttributes|ChaosPenetration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ChaosPenetration;
	
	/** The current Fire Damage Multiplier attribute. Fire Damage Multiplier is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireDamageMultiplier, Category = "Obsidian|CAttributes|FireDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FireDamageMultiplier;

	/** The current Cold Damage Multiplier attribute. Cold Damage Multiplier is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ColdDamageMultiplier, Category = "Obsidian|CAttributes|ColdDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ColdDamageMultiplier;

	/** The current Lightning Damage Multiplier attribute. Lightning Damage Multiplier is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningDamageMultiplier, Category = "Obsidian|CAttributes|LightningDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LightningDamageMultiplier;

	/** The current Elemental Damage Multiplier attribute. Elemental Damage Multiplier is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ElementalDamageMultiplier, Category = "Obsidian|CAttributes|ElementalDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ElementalDamageMultiplier;

	/** The current Chaos Damage Multiplier attribute. Chaos Damage Multiplier is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChaosDamageMultiplier, Category = "Obsidian|CAttributes|ChaosDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ChaosDamageMultiplier;

	/** The current Physical Damage Multiplier attribute. Physical Damage Multiplier is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalDamageMultiplier, Category = "Obsidian|CAttributes|PhysicalDamageMultiplier", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData PhysicalDamageMultiplier;
	
	/**
	 * Base Attributes
	 */
	
	/** The current Physical Damage Multiplier attribute. Physical Damage Multiplier is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Obsidian|CAttributes|BaseDamage", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	/**
	 * Character
	 */

	/** The current Movement Speed attribute. Movement Speed is an attribute since Gameplay Effects can modify it. */
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


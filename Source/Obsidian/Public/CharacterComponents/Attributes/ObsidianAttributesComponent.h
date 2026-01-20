// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"

#include "Components/ActorComponent.h"
#include "ObsidianAttributesComponent.generated.h"

class AObsidianHero;
class UObsidianHeroAttributeSet;
class UObsidianCommonAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObsidianAttrComp_DeathEvent, AActor*, OwningActor);

/**
 * EObsidianDeathState
 *
 * Defines the state of death.
 */
UENUM(BlueprintType)
enum class EObsidianDeathState : uint8
{
	EDS_Alive = 0 UMETA(DisplayName = "Alive"),
	EDS_DeathStarted UMETA(DisplayName = "Death Started"),
	EDS_DeathFinished UMETA(DisplayName = "Death Finished")
};

/**
 * UObsidianAttributesComponent
 *
 * Base Attributes Component class for this project - Hero and Enemy Attributes Component classes should derive from this.
 */
UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OBSIDIAN_API UObsidianAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianAttributesComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the COMMON Attributes Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|EnemyAttributes")
	static UObsidianAttributesComponent* FindCommonAttributesComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianAttributesComponent>() : nullptr);
	}

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Attributes")
	EObsidianDeathState GetDeathState() const
	{
		return DeathState;
	}

	/** Returns true if the owning character is dead or in the process of dying. */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ExpandBoolAsExecs), Category = "Obsidian|Attributes")
	bool IsDeadOrDying() const
	{
		return DeathState > EObsidianDeathState::EDS_Alive;
	}
	
	/** Initializes this component using ASC. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Attributes")
	virtual void InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, ACharacter* Owner = nullptr);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Uninitialize this component, clearing any references to the ASC. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Attributes")
	virtual void UninitializeFromAbilitySystem();
	
	/** Start the death sequence for the owner. */
	virtual void StartDeath();

	/** Ends the death sequence for the owner. */
	virtual void FinishDeath();
	
	/**
	 * Getters for Gameplay Attributes.
	 */

	/** Getters for Health Value and Attribute from UObsidianCommonAttributeSet. */
	float GetHealth() const;
	FGameplayAttribute GetHealthAttribute() const;

	/** Getters for Max Health Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMaxHealth() const;
	FGameplayAttribute GetMaxHealthAttribute() const;

	/** Getters for Energy Shield Value and Attribute from UObsidianCommonAttributeSet. */
	float GetEnergyShield() const;
	FGameplayAttribute GetEnergyShieldAttribute() const;

	/** Getters for Max Energy Shield Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMaxEnergyShield() const;
	FGameplayAttribute GetMaxEnergyShieldAttribute() const;

	/** Getters for Health Regeneration Value and Attribute from UObsidianCommonAttributeSet. */
	float GetHealthRegeneration() const;
	FGameplayAttribute GetHealthRegenerationAttribute() const;

	/** Getters for Energy Shield Regeneration Value and Attribute from UObsidianCommonAttributeSet. */
	float GetEnergyShieldRegeneration() const;
	FGameplayAttribute GetEnergyShieldRegenerationAttribute() const;

	/** Getters for Stagger Meter Value and Attribute from UObsidianCommonAttributeSet. */
	float GetStaggerMeter() const;
	FGameplayAttribute GetStaggerMeterAttribute() const;

	/** Getters for Max Stagger Meter Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMaxStaggerMeter() const;
	FGameplayAttribute GetMaxStaggerMeterAttribute() const;

	/** Getters for Stagger Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMaxStaggerMultiplier() const;
	FGameplayAttribute GetMaxStaggerMultiplierAttribute() const;

	/** Getters for All Damage Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetAllDamageMultiplier() const;
	FGameplayAttribute GetAllDamageMultiplierAttribute() const;
	
	/** Getters for Armor Value and Attribute from UObsidianCommonAttributeSet. */
	float GetArmor() const;
	FGameplayAttribute GetArmorAttribute() const;

	/** Getters for Evasion Value and Attribute from UObsidianCommonAttributeSet. */
	float GetEvasion() const;
	FGameplayAttribute GetEvasionAttribute() const;

	/** Getters for Spell Suppression Chance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetSpellSuppressionChance() const;
	FGameplayAttribute GetSpellSuppressionChanceAttribute() const;

	/** Getters for Spell Suppression Magnitude Value and Attribute from UObsidianCommonAttributeSet. */
	float GetSpellSuppressionMagnitude() const;
	FGameplayAttribute GetSpellSuppressionMagnitudeAttribute() const;

	/** Getters for Ailment Threshold Value and Attribute from UObsidianCommonAttributeSet. */
	float GetAilmentThreshold() const;
	FGameplayAttribute GetAilmentThresholdAttribute() const;

	/** Getters for Shock Damage Taken Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetShockDamageTakenMultiplier() const;
	FGameplayAttribute GetShockDamageTakenMultiplierAttribute() const;
	
	/** Getters for Stagger Damage Taken Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetStaggerDamageTakenMultiplier() const;
	FGameplayAttribute GetStaggerDamageTakenMultiplierAttribute() const;

	/** Getters for All Elemental Resistances Value and Attribute from UObsidianCommonAttributeSet. */
	float GetAllElementalResistances() const;
	FGameplayAttribute GetAllElementalResistancesAttribute() const;
	
	/** Getters for Fire Resistance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetFireResistance() const;
	FGameplayAttribute GetFireResistanceAttribute() const;
	
	/** Getters for Max Fire Resistance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMaxFireResistance() const;
	FGameplayAttribute GetMaxFireResistanceAttribute() const;

	/** Getters for Cold Resistance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetColdResistance() const;
	FGameplayAttribute GetColdResistanceAttribute() const;

	/** Getters for Max Cold Resistance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMaxColdResistance() const;
	FGameplayAttribute GetMaxColdResistanceAttribute() const;

	/** Getters for Lightning Resistance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetLightningResistance() const;
	FGameplayAttribute GetLightningResistanceAttribute() const;

	/** Getters for Max Lightning Resistance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMaxLightningResistance() const;
	FGameplayAttribute GetMaxLightningResistanceAttribute() const;

	/** Getters for Chaos Resistance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetChaosResistance() const;
	FGameplayAttribute GetChaosResistanceAttribute() const;

	/** Getters for Max Chaos Resistance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMaxChaosResistance() const;
	FGameplayAttribute GetMaxChaosResistanceAttribute() const;

	/** Getters for Chance To Shock Value and Attribute from UObsidianCommonAttributeSet. */
	float GetChanceToShock() const;
	FGameplayAttribute GetChanceToShockAttribute() const;

	/** Getters for Increased Effect Of Shock Value and Attribute from UObsidianCommonAttributeSet. */
	float GetIncreasedEffectOfShock() const;
	FGameplayAttribute GetIncreasedEffectOfShockAttribute() const;

	/** Getters for Critical Strike Chance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetAccuracy() const;
	FGameplayAttribute GetAccuracyAttribute() const;

	/** Getters for Critical Strike Chance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetCriticalStrikeChance() const;
	FGameplayAttribute GetCriticalStrikeChanceAttribute() const;

	/** Getters for Critical Strike Damage Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetCriticalStrikeDamageMultiplier() const;
	FGameplayAttribute GetCriticalStrikeDamageMultiplierAttribute() const;

	/** Getters for Critical Strike Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetAttackSpeed() const;
	FGameplayAttribute GetAttackSpeedAttribute() const;

	/** Getters for Critical Strike Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetCastSpeed() const;
	FGameplayAttribute GetCastSpeedAttribute() const;

	/** Getters for Fire Penetration Value and Attribute from UObsidianCommonAttributeSet. */
	float GetFirePenetration() const;
	FGameplayAttribute GetFirePenetrationAttribute() const;

	/** Getters for Cold Penetration Value and Attribute from UObsidianCommonAttributeSet. */
	float GetColdPenetration() const;
	FGameplayAttribute GetColdPenetrationAttribute() const;

	/** Getters for Lightning Penetration Value and Attribute from UObsidianCommonAttributeSet. */
	float GetLightningPenetration() const;
	FGameplayAttribute GetLightningPenetrationAttribute() const;

	/** Getters for Elemental Penetration Value and Attribute from UObsidianCommonAttributeSet. */
	float GetAllElementalPenetration() const;
	FGameplayAttribute GetAllElementalPenetrationAttribute() const;

	/** Getters for Chaos Penetration Value and Attribute from UObsidianCommonAttributeSet. */
	float GetChaosPenetration() const;
	FGameplayAttribute GetChaosPenetrationAttribute() const;

	/** Getters for Fire Damage Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetFireDamageMultiplier() const;
	FGameplayAttribute GetFireDamageMultiplierAttribute() const;

	/** Getters for Cold Damage Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetColdDamageMultiplier() const;
	FGameplayAttribute GetColdDamageMultiplierAttribute() const;

	/** Getters for Lightning Damage Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetLightningDamageMultiplier() const;
	FGameplayAttribute GetLightningDamageMultiplierAttribute() const;

	/** Getters for All Elemental Damage Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetAllElementalDamageMultiplier() const;
	FGameplayAttribute GetAllElementalDamageMultiplierAttribute() const;

	/** Getters for Chaos Damage Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetChaosDamageMultiplier() const;
	FGameplayAttribute GetChaosDamageMultiplierAttribute() const;

	/** Getters for Physical Damage Multiplier Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetPhysicalDamageMultiplier() const;
	FGameplayAttribute GetPhysicalDamageMultiplierAttribute() const;

	/** Getters for Min Flat Physical Damage Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetMinFlatPhysicalDamage() const;
	FGameplayAttribute GetMinFlatPhysicalDamageAttribute() const;

	/** Getters for Max Flat Physical Damage Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetMaxFlatPhysicalDamage() const;
	FGameplayAttribute GetMaxFlatPhysicalDamageAttribute() const;

	/** Getters for Min Flat Fire Damage Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetMinFlatFireDamage() const;
	FGameplayAttribute GetMinFlatFireDamageAttribute() const;

	/** Getters for Max Flat Fire Damage Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetMaxFlatFireDamage() const;
	FGameplayAttribute GetMaxFlatFireDamageAttribute() const;

	/** Getters for Min Flat Cold Damage and Attribute from UObsidianCommonAttributeSet. */	
	float GetMinFlatColdDamage() const;
	FGameplayAttribute GetMinFlatColdDamageAttribute() const;

	/** Getters for Min Flat Cold Damage Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetMaxFlatColdDamage() const;
	FGameplayAttribute GetMaxFlatColdDamageAttribute() const;

	/** Getters for Min Flat Lightning Damage Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetMinFlatLightningDamage() const;
	FGameplayAttribute GetMinFlatLightningDamageAttribute() const;

	/** Getters for Max Flat Lightning Damage Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetMaxFlatLightningDamage() const;
	FGameplayAttribute GetMaxFlatLightningDamageAttribute() const;

	/** Getters for Min Flat Chaos Damage Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetMinFlatChaosDamage() const;
	FGameplayAttribute GetMinFlatChaosDamageAttribute() const;

	/** Getters for Max Flat Chaos Damage Value and Attribute from UObsidianCommonAttributeSet. */	
	float GetMaxFlatChaosDamage() const;
	FGameplayAttribute GetMaxFlatChaosDamageAttribute() const;
	
	/** Getters for Movement Speed Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMovementSpeed() const;
	FGameplayAttribute GetMovementSpeedAttribute() const;
	

public:
	/** Delegate fired when the death sequence has started. */
	UPROPERTY(BlueprintAssignable)
	FObsidianAttrComp_DeathEvent OnDeathStarted;

	/** Delegate fired when the death sequence has finished. */ 
	UPROPERTY(BlueprintAssignable)
	FObsidianAttrComp_DeathEvent OnDeathFinished;

protected:
	virtual void OnUnregister() override;
	virtual void ClearGameplayTags();
	
	/**
	 * Callbacks for Attribute change delegates.
	 */
	
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void EnergyShieldChanged(const FOnAttributeChangeData& Data);
	virtual void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data);
	
	/**
	 *  Death Sequence
	 */

	UFUNCTION()
	virtual void OnRep_DeathState(EObsidianDeathState OldDeathState);

	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	
protected:
	/** Ability System used by this component. */
	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> AbilitySystemComponent;

	/**
	 * Sets used by this component.
	 */
	
	UPROPERTY()
	TObjectPtr<const UObsidianCommonAttributeSet> CommonAttributeSet;
	
	/**
	 * Attribute change delegate handles.
	 */

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle EnergyShieldChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
	
	/**
	 *
	 */

	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EObsidianDeathState DeathState;
};

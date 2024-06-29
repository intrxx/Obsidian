// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Components/ActorComponent.h"
#include "ObsidianAttributesComponent.generated.h"

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
	
	/** Initializes this component using ASC. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Attributes")
	virtual void InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner = nullptr);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Obsidian|EnemyAttributes")
	static UObsidianAttributesComponent* FindCommonAttributesComponent(const AActor* Actor) {return (Actor ? Actor->FindComponentByClass<UObsidianAttributesComponent>() : nullptr);}
	
	/** Uninitialize this component, clearing any references to the ASC. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Attributes")
	virtual void UninitializeFromAbilitySystem();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Attributes")
	EObsidianDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ExpandBoolAsExecs), Category = "Obsidian|Attributes")
	bool IsDeadOrDying() const { return DeathState > EObsidianDeathState::EDS_Alive; }

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

	/** Getters for Critical Strike Chance Value and Attribute from UObsidianCommonAttributeSet. */
	float GetCriticalStrikeChance() const;
	FGameplayAttribute GetCriticalStrikeChanceAttribute() const;

	/** Getters for Critical Strike Multiplier Value and Attribute from UObsidianCommonAttributeSet. */
	float GetCriticalStrikeMultiplier() const;
	FGameplayAttribute GetCriticalStrikeMultiplierAttribute() const;

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



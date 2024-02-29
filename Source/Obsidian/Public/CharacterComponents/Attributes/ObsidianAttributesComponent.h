// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Components/ActorComponent.h"
#include "ObsidianAttributesComponent.generated.h"

class UObsidianHeroAttributeSet;
class UObsidianCommonAttributeSet;

/**
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

	/** Uninitialize this component, clearing any references to the ASC. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Attributes")
	virtual void UninitializeFromAbilitySystem();

	/**
	 * Getters for Gameplay Attributes.
	 */

	/** Getters for Health Value and Attribute from UObsidianCommonAttributeSet. */
	float GetHealth() const;
	FGameplayAttribute GetHealthAttribute() const;

	/** Getters for Max Health Value and Attribute from UObsidianCommonAttributeSet. */
	float GetMaxHealth() const;
	FGameplayAttribute GetMaxHealthAttribute() const;
	
	/** Getters for Armor Value and Attribute from UObsidianCommonAttributeSet. */
	float GetArmor() const;
	FGameplayAttribute GetArmorAttribute() const;

	/** Getters for Evasion Value and Attribute from UObsidianCommonAttributeSet. */
	float GetEvasion() const;
	FGameplayAttribute GetEvasionAttribute() const;

	/** Getters for Energy Shield Value and Attribute from UObsidianCommonAttributeSet. */
	float GetEnergyShield() const;
	FGameplayAttribute GetEnergyShieldAttribute() const;

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
	
	/**
	 *
	 */

protected:
	virtual void OnUnregister() override;
	
	/**
	 * Callbacks for Attribute change delegates.
	 */
	
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ArmorChanged(const FOnAttributeChangeData& Data);
	virtual void EvasionChanged(const FOnAttributeChangeData& Data);
	virtual void EnergyShieldChanged(const FOnAttributeChangeData& Data);
	virtual void FireResistanceChanged(const FOnAttributeChangeData& Data);
	virtual void MaxFireResistanceChanged(const FOnAttributeChangeData& Data);
	virtual void ColdResistanceChanged(const FOnAttributeChangeData& Data);
	virtual void MaxColdResistanceChanged(const FOnAttributeChangeData& Data);
	virtual void LightningResistanceChanged(const FOnAttributeChangeData& Data);
	virtual void MaxLightningResistanceChanged(const FOnAttributeChangeData& Data);
	virtual void ChaosResistanceChanged(const FOnAttributeChangeData& Data);
	virtual void MaxChaosResistanceChanged(const FOnAttributeChangeData& Data);
	
	/**
	 * 
	 */
	
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
	FDelegateHandle ArmorChangedDelegateHandle;
	FDelegateHandle EvasionChangedDelegateHandle;
	FDelegateHandle EnergyShieldChangedDelegateHandle;
	FDelegateHandle FireResistanceChangedDelegateHandle;
	FDelegateHandle MaxFireResistanceChangedDelegateHandle;
	FDelegateHandle ColdResistanceChangedDelegateHandle;
	FDelegateHandle MaxColdResistanceChangedDelegateHandle;
	FDelegateHandle LightningResistanceChangedDelegateHandle;
	FDelegateHandle MaxLightningResistanceChangedDelegateHandle;
	FDelegateHandle ChaosResistanceChangedDelegateHandle;
	FDelegateHandle MaxChaosResistanceChangedDelegateHandle;
	
	/**
	 *
	 */
};

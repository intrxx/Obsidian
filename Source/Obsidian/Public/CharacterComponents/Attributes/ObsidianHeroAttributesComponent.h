// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianAttributesComponent.h"
#include "ObsidianHeroAttributesComponent.generated.h"

class UMainOverlayWidgetController;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroAttributesComponent : public UObsidianAttributesComponent
{
	GENERATED_BODY()

public:
	UObsidianHeroAttributesComponent(const FObjectInitializer& ObjectInitializer);
	
	/** Returns the HERO Attributes Component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|HeroAttributes")
	static UObsidianHeroAttributesComponent* FindAttributesComponent(const AActor* Actor) {return (Actor ? Actor->FindComponentByClass<UObsidianHeroAttributesComponent>() : nullptr);}

	//~ Start of ObsidianAttributesComponent
	virtual void InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner = nullptr) override;
	virtual void UninitializeFromAbilitySystem() override;
	//~ End of ObsidianAttributesComponent
	
	/**
	 * Getters for Gameplay Attributes.
	 */

	/** Getters for Mana Value and Attribute from UObsidianHeroAttributeSet. */
	float GetMana() const;
	FGameplayAttribute GetManaAttribute() const;

	/** Getters for Max Mana Value and Attribute from UObsidianHeroAttributeSet. */
	float GetMaxMana() const;
	FGameplayAttribute GetMaxManaAttribute() const;

	/** Getters for Special Resource Value and Attribute from UObsidianHeroAttributeSet. */
	float GetSpecialResource() const;
	FGameplayAttribute GetSpecialResourceAttribute() const;

	/** Getters for Max Special Resource Value and Attribute from UObsidianHeroAttributeSet. */
	float GetMaxSpecialResource() const;
	FGameplayAttribute GetMaxSpecialResourceAttribute() const;

	/** Getters for Mana Regeneration Value and Attribute from UObsidianHeroAttributeSet. */
	float GetManaRegeneration() const;
	FGameplayAttribute GetManaRegenerationAttribute() const;
	
	/** Getters for Strength Value and Attribute from UObsidianHeroAttributeSet. */
	float GetStrength() const;
	FGameplayAttribute GetStrengthAttribute() const;

	/** Getters for Intelligence Value and Attribute from UObsidianHeroAttributeSet. */
	float GetIntelligence() const;
	FGameplayAttribute GetIntelligenceAttribute() const;

	/** Getters for Dexterity Value and Attribute from UObsidianHeroAttributeSet. */
	float GetDexterity() const;
	FGameplayAttribute GetDexterityAttribute() const;

	/** Getters for Faith Value and Attribute from UObsidianHeroAttributeSet. */
	float GetFaith() const;
	FGameplayAttribute GetFaithAttribute() const;

	/**
	 * 
	 */

protected:
	virtual void ClearGameplayTags() override;
	
	/**
	 * Callbacks for Attribute change delegates. //TODO Decide what should actually be bind later - check performance
	 */
	
	virtual void HealthChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data) override;
	virtual void EnergyShieldChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) override;
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	
	/**
	 * 
	 */

protected:
	/**
	 * Widget controller that will communicate with UI.
	 */
	UPROPERTY()
	TObjectPtr<UMainOverlayWidgetController> MainOverlayWidgetController;
	
	/**
	 * Sets used by this component.
	 */
	
	UPROPERTY()
	TObjectPtr<const UObsidianHeroAttributeSet> HeroAttributeSet;

	/**
	 * Attribute change delegate handles.
	 */

	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;

	/**
	 * 
	 */
};

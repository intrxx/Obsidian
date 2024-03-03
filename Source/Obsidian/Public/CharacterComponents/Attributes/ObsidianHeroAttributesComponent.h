// Copyright 2024 Michał Ogiński

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

	void SetMainWidgetController(UMainOverlayWidgetController* InWidgetController);
	
	/**
	 * Getters for Gameplay Attributes.
	 */

	/** Getters for Mana Value and Attribute from UObsidianHeroAttributeSet. */
	float GetMana() const;
	FGameplayAttribute GetManaAttribute() const;

	/** Getters for Max Mana Value and Attribute from UObsidianHeroAttributeSet. */
	float GetMaxMana() const;
	FGameplayAttribute GetMaxManaAttribute() const;
	
	/** Getters for Strength Value and Attribute from UObsidianHeroAttributeSet. */
	float GetStrength() const;
	FGameplayAttribute GetStrengthAttribute() const;

	/** Getters for Intelligence Value and Attribute from UObsidianHeroAttributeSet. */
	float GetIntelligence() const;
	FGameplayAttribute GetIntelligenceAttribute() const;

	/** Getters for Dexterity Value and Attribute from UObsidianHeroAttributeSet. */
	float GetDexterity() const;
	FGameplayAttribute GetDexterityAttribute() const;

	/**
	 * 
	 */

protected:
	/**
	 * Callbacks for Attribute change delegates.
	 */
	
	virtual void HealthChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data) override;
	virtual void EnergyShieldChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) override;
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void StrengthChanged(const FOnAttributeChangeData& Data);
	virtual void IntelligenceChanged(const FOnAttributeChangeData& Data);
	virtual void DexterityChanged(const FOnAttributeChangeData& Data);
	virtual void ArmorChanged(const FOnAttributeChangeData& Data) override;
	virtual void EvasionChanged(const FOnAttributeChangeData& Data) override;
	virtual void FireResistanceChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxFireResistanceChanged(const FOnAttributeChangeData& Data) override;
	virtual void ColdResistanceChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxColdResistanceChanged(const FOnAttributeChangeData& Data) override;
	virtual void LightningResistanceChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxLightningResistanceChanged(const FOnAttributeChangeData& Data) override;
	virtual void ChaosResistanceChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxChaosResistanceChanged(const FOnAttributeChangeData& Data) override;
	virtual void CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data) override;
	virtual void CriticalStrikeMultiplierChanged(const FOnAttributeChangeData& Data) override;
	
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
	FDelegateHandle StrengthChangedDelegateHandle;
	FDelegateHandle IntelligenceChangedDelegateHandle;
	FDelegateHandle DexterityChangedDelegateHandle;

	/**
	 * 
	 */
};

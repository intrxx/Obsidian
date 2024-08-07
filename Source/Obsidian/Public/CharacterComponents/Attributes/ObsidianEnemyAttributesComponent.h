// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"
#include "ObsidianEnemyAttributesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyAttributeValueChangedSignature, const float /** New Value */);

class UObsidianEnemyAttributeSet;
/**
 * Class that handles all attributes that are given to Enemy classes.
 */
UCLASS()
class OBSIDIAN_API UObsidianEnemyAttributesComponent : public UObsidianAttributesComponent
{
	GENERATED_BODY()

public:
	UObsidianEnemyAttributesComponent(const FObjectInitializer& ObjectInitializer);
	
	/** Returns the ENEMY Attributes Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|EnemyAttributes")
	static UObsidianEnemyAttributesComponent* FindAttributesComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianEnemyAttributesComponent>() : nullptr);
	}

	//~ Start of ObsidianAttributesComponent
	virtual void InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner = nullptr) override;
	virtual void UninitializeFromAbilitySystem() override;
	//~ End of ObsidianAttributesComponent
	
	/**
	 * Getters for Gameplay Attributes.
	 */
	
	/** Getters for EnemySpecificAttribute Value and Attribute from UObsidianEnemyAttributeSet. */
	float GetHitReactThreshold() const;
	FGameplayAttribute GetHitReactThresholdAttribute() const;

	/**
	 * Delegates for enemy health bar
	 */
	
	FOnEnemyAttributeValueChangedSignature HealthChangedDelegate;
	FOnEnemyAttributeValueChangedSignature MaxHealthChangedDelegate;
	FOnEnemyAttributeValueChangedSignature EnergyShieldChangedDelegate;
	FOnEnemyAttributeValueChangedSignature MaxEnergyShieldChangedDelegate;

protected:
	virtual void ClearGameplayTags() override;
	
	/**
	 * Callbacks for Attribute change delegates.
	 */
	
	virtual void HealthChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data) override;
	virtual void EnergyShieldChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) override;

	/**
	 * 
	 */

protected:
	/**
	 * Sets used by this component.
	 */
	
	UPROPERTY()
	TObjectPtr<const UObsidianEnemyAttributeSet> EnemyAttributeSet;

	/**
	 * Attribute change delegate handles.
	 */

	/**
	 * 
	 */

private:
	void BroadcastInitialValues() const;
	
};

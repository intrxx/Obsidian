// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"

#include "ObsidianEnemyAttributesComponent.generated.h"

struct FObsidianEffectUIData;
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
	UObsidianEnemyAttributesComponent(const FObjectInitializer& InObjectInitializer);
	
	/** Returns the ENEMY Attributes Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|EnemyAttributes")
	static UObsidianEnemyAttributesComponent* FindAttributesComponent(const AActor* InActor)
	{
		return (InActor ? InActor->FindComponentByClass<UObsidianEnemyAttributesComponent>() : nullptr);
	}

	void SetEnemyName(const FText& InEnemyName)
	{
		EnemyName = InEnemyName;
	}
	
	/**
	 * Getters for Gameplay Attributes.
	 */

	FText GetEnemyName() const
	{
		return EnemyName;
	}
	
	
	/** Getters for EnemySpecificAttribute Value and Attribute from UObsidianEnemyAttributeSet. */
	float GetHitReactThreshold() const;
	FGameplayAttribute GetHitReactThresholdAttribute() const;
	
	//~ Start of ObsidianAttributesComponent
	virtual void InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, ACharacter* InOwner = nullptr) override;
	virtual void UninitializeFromAbilitySystem() override;
	//~ End of ObsidianAttributesComponent

public:
	/**
	 * Delegates for enemy health bar
	 */
	
	FOnEnemyAttributeValueChangedSignature HealthChangedDelegate;
	FOnEnemyAttributeValueChangedSignature MaxHealthChangedDelegate;
	FOnEnemyAttributeValueChangedSignature EnergyShieldChangedDelegate;
	FOnEnemyAttributeValueChangedSignature MaxEnergyShieldChangedDelegate;
	FOnEnemyAttributeValueChangedSignature StaggerMeterChangedDelegate;
	FOnEnemyAttributeValueChangedSignature MaxStaggerMeterChangedDelegate;

protected:
	virtual void ClearGameplayTags() override;
	
	/**
	 * Callbacks for Attribute change delegates.
	 */
	
	virtual void HealthChanged(const FOnAttributeChangeData& InData) override;
	virtual void MaxHealthChanged(const FOnAttributeChangeData& InData) override;
	virtual void EnergyShieldChanged(const FOnAttributeChangeData& InData) override;
	virtual void MaxEnergyShieldChanged(const FOnAttributeChangeData& InData) override;
	virtual void StaggerMeterChanged(const FOnAttributeChangeData& InData);
	virtual void MaxStaggerMeterChanged(const FOnAttributeChangeData& InData);

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

	FDelegateHandle StaggerMeterDelegateHandle;
	FDelegateHandle MaxStaggerMeterDelegateHandle;

	/**
	 * 
	 */

private:
	FText EnemyName = FText::FromString("Lorem");
	
private:
	void BroadcastInitialValues() const;
	
};

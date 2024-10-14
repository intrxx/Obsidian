// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianTypes/ObsidianUITypes.h"
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
	UObsidianEnemyAttributesComponent(const FObjectInitializer& ObjectInitializer);
	
	/** Returns the ENEMY Attributes Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|EnemyAttributes")
	static UObsidianEnemyAttributesComponent* FindAttributesComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianEnemyAttributesComponent>() : nullptr);
	}

	void SetEnemyName(const FText& InEnemyName)
	{
		EnemyName = InEnemyName;
	}

	void SetUIDataTable(UDataTable* InDataTable)
	{
		UIEffectDataWidgetTable = InDataTable;
	};

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
	virtual void InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner = nullptr) override;
	virtual void UninitializeFromAbilitySystem() override;
	//~ End of ObsidianAttributesComponent
	
	bool BindToOnEffectCallback();
	void ClearOnEffectCallback();

public:
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FStackingEffectUIDataWidgetRow EffectStackingUIDataDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FEffectUIDataWidgetRow EffectUIDataWidgetRowDelegate;
	
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
	
	virtual void HealthChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data) override;
	virtual void EnergyShieldChanged(const FOnAttributeChangeData& Data) override;
	virtual void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) override;
	virtual void StaggerMeterChanged(const FOnAttributeChangeData& Data);
	virtual void MaxStaggerMeterChanged(const FOnAttributeChangeData& Data);

	/**
	 * 
	 */
	
	void HandleEnemyEffectApplied(const FObsidianEffectUIData& UIData);

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

	FDelegateHandle UIDataDelegateHandle;
	
private:
	UPROPERTY()
	TObjectPtr<UDataTable> UIEffectDataWidgetTable;
	
	FText EnemyName = FText::FromString("Lorem");
	
private:
	void BroadcastInitialValues() const;
	
};

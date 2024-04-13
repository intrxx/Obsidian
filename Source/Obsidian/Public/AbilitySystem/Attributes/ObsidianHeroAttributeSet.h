// Copyright 2024 Michał Ogiński

#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/ObsidianAttributeSetBase.h"
#include "ObsidianHeroAttributeSet.generated.h"

/**
 * Attribute set specific to the hero (player character)
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroAttributeSet : public UObsidianAttributeSetBase
{
	GENERATED_BODY()

public:
	UObsidianHeroAttributeSet();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/**
	 * Spending attributes
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, MaxMana);

	/**
	 * "RPG Attributes"
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Strength);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Intelligence);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Dexterity);

protected:
	
	/**
	 * Spending attributes
	 */
	
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
	
	/**
	 * "RPG Attributes"
	 */
	
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Dexterity(const FGameplayAttributeData& OldValue);

private:
	/**
	 * Spending attributes
	 */
	
	/** The current Mana attribute. The Mana will be capped by the Max Mana attribute. Health is hidden from modifiers so only Executions can modify it. */
	/** ------------------------------------------------------------------------------------------------------ HideFromModifiers */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Obsidian|HAttributes|Mana", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Mana;
	
	/** The current Max Mana attribute. Max Mana is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Obsidian|HAttributes|MaxMana", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMana;

	/**
	 * "RPG Attributes"
	 */
	
	/** The current Strength attribute. Strength is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Obsidian|HAttributes|Strength", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Strength;

	/** The current Intelligence attribute. Intelligence is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Obsidian|HAttributes|Intelligence", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Intelligence;

	/** The current Dexterity attribute. Dexterity is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Dexterity, Category = "Obsidian|HAttributes|Dexterity", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Dexterity;
};

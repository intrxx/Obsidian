// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

// ~ Project


#include "ObsidianCommonAttributeSet.h"
#include "ObsidianHeroAttributeSet.generated.h"

/**
 * Attribute set specific to the hero (player character)
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroAttributeSet : public UObsidianCommonAttributeSet
{
	GENERATED_BODY()

public:
	UObsidianHeroAttributeSet();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	static float GetMaxExperienceForLevel(const uint8 HeroLevel);

	/**
	 *	Character
	 */

	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Experience);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, MaxExperience);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, PassiveSkillPoints);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, AscensionPoints);

	/**
	 * Spending attributes
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, MaxMana);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, IncreasedManaPercentage);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, SpecialResource);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, MaxSpecialResource);

	/**
	 * Status
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, ManaRegeneration);

	/**
	 * "RPG Attributes"
	 */
	
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Strength);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Intelligence);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Dexterity);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, Faith);

	/**
	 * Defence Attributes
	 */

	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, HitBlockChance);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, MaxHitBlockChance);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, SpellBlockChance);
	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, MaxSpellBlockChance);

	/**
	 * Meta Attributes
	 */

	ATTRIBUTE_ACCESSORS(UObsidianHeroAttributeSet, IncomingManaReplenishing);

protected:
	virtual void ResetMetaAttributes() override;
	
	/**
	 *	Character
	 */
	
	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxExperience(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_PassiveSkillPoints(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_AscensionPoints(const FGameplayAttributeData& OldValue);
	
	/**
	 * Spending attributes
	 */
	
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_IncreasedManaPercentage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SpecialResource(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxSpecialResource(const FGameplayAttributeData& OldValue);

	/**
	 * Status
	 */

	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldValue);

	/**
	 * Defence Attributes
	 */

	UFUNCTION()
	void OnRep_HitBlockChance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHitBlockChance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SpellBlockChance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxSpellBlockChance(const FGameplayAttributeData& OldValue);
	
	/**
	 * "RPG Attributes"
	 */
	
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Dexterity(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Faith(const FGameplayAttributeData& OldValue);

private:
	/**
	 * Character
	 */
	
	/** The current Experience attribute. It represents current experience. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Experience, Category = "Obsidian|CAttributes|Experience", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Experience;

	/** The current Max Experience attribute. Defines Experience that we need in order to level up. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxExperience, Category = "Obsidian|CAttributes|Experience", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxExperience;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PassiveSkillPoints, Category = "Obsidian|CAttributes|PassiveSkillPoints", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData PassiveSkillPoints;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AscensionPoints, Category = "Obsidian|CAttributes|AscensionPoints", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AscensionPoints;
	
	/**
	 * Spending attributes
	 */
	
	/** The current Mana attribute. The Mana will be capped by the Max Mana attribute. Health is hidden from modifiers so only Executions can modify it. */
	/** ------------------------------------------------------------------------------------------------------ HideFromModifiers */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Obsidian|HAttributes|Mana", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Mana;
	
	/** The current Max Mana attribute. Max Mana is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Obsidian|HAttributes|Mana", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMana;

	/** Kind of Meta attribute - The current Increased Mana Percentage attribute. Used in calculation for increasing Max Health attribute. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IncreasedManaPercentage, Category = "Obsidian|CAttributes|Mana", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncreasedManaPercentage;

	/** The current Special Resource attribute. The Special Resource will be capped by the Max Special Resource attribute. Special Resource is hidden from modifiers so only Executions can modify it. */
	/** ------------------------------------------------------------------------------------------------------ HideFromModifiers */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpecialResource, Category = "Obsidian|HAttributes|SpecialResource", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SpecialResource;
	
	/** The current Max Special Resource. Max Special Resource is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxSpecialResource, Category = "Obsidian|HAttributes|SpecialResource", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSpecialResource;

	/**
	 * Status
	 */

	/** The current Mana Regeneration attribute. Mana Regeneration defines value of mana that will be regenerated per second */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category = "Obsidian|CAttributes|ManaRegeneration", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ManaRegeneration;
	
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

	/** The current Faith attribute. Faith is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Faith, Category = "Obsidian|HAttributes|Faith", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Faith;

	/**
	 * Defence Attributes
	 */

	/** The current Hit Block Chance attribute. Hit Block Chance determines a chance to block hit damage (e.g. attacks, non-spell projectiles) */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitBlockChance, Category = "Obsidian|HAttributes|HitBlockChance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HitBlockChance;

	/** The current Max Hit Block Chance attribute. Max Hit Block Chance determines a cap for Hit Block Chance */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHitBlockChance, Category = "Obsidian|HAttributes|HitBlockChance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHitBlockChance;

	/** The current Spell Block Chance attribute. Spell Block Chance determines a chance to block spell damage */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpellBlockChance, Category = "Obsidian|HAttributes|SpellBlockChance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SpellBlockChance;
	
	/** The current Max Spell Block Chance attribute. Max Spell Block Chance determines a cap for Spell Block Chance */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxSpellBlockChance, Category = "Obsidian|HAttributes|SpellBlockChance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSpellBlockChance;
	
	/**
	 * Meta Attributes
	 */

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HAttributes|Meta", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingManaReplenishing;
};

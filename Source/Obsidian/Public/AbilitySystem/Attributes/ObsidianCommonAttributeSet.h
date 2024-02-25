// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianAttributeSetBase.h"
#include "ObsidianCommonAttributeSet.generated.h"

/**
 * Attribute set that holds all the shared attributes between enemies and the player character
 */
UCLASS()
class OBSIDIAN_API UObsidianCommonAttributeSet : public UObsidianAttributeSetBase
{
	GENERATED_BODY()

public:
	UObsidianCommonAttributeSet();

	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, Evasion);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, EnergyShield);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, FireResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ColdResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, LightningResistance);
	ATTRIBUTE_ACCESSORS(UObsidianCommonAttributeSet, ChaosResistance);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Evasion(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_EnergyShield(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ColdResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ChaosResistance(const FGameplayAttributeData& OldValue);

private:
	/** The current Health attribute. The Health will be capped by the Max Health attribute. Health is hidden from modifiers so only Executions can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Obsidian|CAttributes|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	/** The current max Health attribute. Max Health is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Obsidian|CAttributes|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	/** The current Armor attribute. Armor is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Obsidian|CAttributes|Armor", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Armor;

	/** The current Evasion attribute. Evasion is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Evasion, Category = "Obsidian|CAttributes|Evasion", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Evasion;

	/** The current Energy Shield attribute. Energy Shield is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyShield, Category = "Obsidian|CAttributes|EnergyShield", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EnergyShield;
	
	/** The current Fire Resistance attribute. Fire Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Obsidian|CAttributes|FireResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FireResistance;

	/** The current Cold Resistance attribute. Cold Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ColdResistance, Category = "Obsidian|CAttributes|ColdResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ColdResistance;

	/** The current Lightning Resistance attribute. Lightning Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Obsidian|CAttributes|LightningResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LightningResistance;

	/** The current Chaos Resistance attribute. Chaos Resistance is an attribute since Gameplay Effects can modify it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChaosResistance, Category = "Obsidian|CAttributes|ChaosResistance", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ChaosResistance;
};

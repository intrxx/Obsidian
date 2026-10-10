// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/ObsidianWidgetControllerBase.h"

#include "ObCharacterStatusWidgetController.generated.h"

struct FOnAttributeChangeData;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UObCharacterStatusWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	// ~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	// ~ End of UObsidianWidgetController
	
	void SetInitialAttributeValues() const;
	
public:
	FText HeroClassText = FText();
	FString HeroNameString = FString();
	
	/** Character */
	FOnHeroLevelUpSignature HeroLevelUpDelegate;
	FOnAttributeValueChangedOneParam ExperienceChangedDelegate;
	FOnAttributeValueChangedTwoParams MaxExperienceChangedDelegate;
	
	/** Attributes */
	FOnAttributeValueChangedOneParam StrengthValueChangedDelegate;
	FOnAttributeValueChangedOneParam IntelligenceValueChangedDelegate;
	FOnAttributeValueChangedOneParam DexterityValueChangedDelegate;
	FOnAttributeValueChangedOneParam FaithValueChangedDelegate;

	/** Vital Attributes */
	FOnAttributeValueChangedOneParam MaxHealthChangedDelegate;
	FOnAttributeValueChangedOneParam MaxManaChangedDelegate;
	FOnAttributeValueChangedOneParam MaxSpecialResourceChangedDelegate;
	FOnAttributeValueChangedOneParam MaxEnergyShieldChangedDelegate;
	FOnAttributeValueChangedOneParam MaxStaminaChangedDelegate;
	FOnAttributeValueChangedOneParam StaminaRegenerationChangedDelegate;
	
	/** Offence */
	FOnAttributeValueChangedOneParam AccuracyChangedDelegate;
	FOnAttributeValueChangedOneParam AttackSpeedChangedDelegate;
	FOnAttributeValueChangedOneParam CastSpeedChangedDelegate;
	FOnAttributeValueChangedOneParam CriticalStrikeChanceChangedDelegate;
	FOnAttributeValueChangedOneParam CriticalStrikeDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam PhysicalDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam FireDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam LightningDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam ColdDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam ChaosDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam FirePenetrationChangedDelegate;
	FOnAttributeValueChangedOneParam LightningPenetrationChangedDelegate;
	FOnAttributeValueChangedOneParam ColdPenetrationChangedDelegate;
	FOnAttributeValueChangedOneParam ChaosPenetrationChangedDelegate;
	
	/** Defence */
	FOnAttributeValueChangedOneParam ArmorChangedDelegate;
	FOnAttributeValueChangedOneParam EvasionChangedDelegate;
	FOnAttributeValueChangedOneParam HealthRegenerationChangedDelegate;
	FOnAttributeValueChangedOneParam EnergyShieldRegenerationChangedDelegate;
	FOnAttributeValueChangedTwoParams FireResistanceChangedDelegate;
	FOnAttributeValueChangedTwoParams MaxFireResistanceChangedDelegate;
	FOnAttributeValueChangedTwoParams ColdResistanceChangedDelegate;
	FOnAttributeValueChangedTwoParams MaxColdResistanceChangedDelegate;
	FOnAttributeValueChangedTwoParams LightningResistanceChangedDelegate;
	FOnAttributeValueChangedTwoParams MaxLightningResistanceChangedDelegate;
	FOnAttributeValueChangedTwoParams ChaosResistanceChangedDelegate;
	FOnAttributeValueChangedTwoParams MaxChaosResistanceChangedDelegate;
	FOnAttributeValueChangedOneParam SpellSuppressionChanceChangedDelegate;
	FOnAttributeValueChangedOneParam SpellSuppressionMagnitudeChangedDelegate;
	FOnAttributeValueChangedTwoParams HitBlockChanceChangedDelegate;
	FOnAttributeValueChangedTwoParams MaxHitBlockChanceChangedDelegate;
	FOnAttributeValueChangedTwoParams SpellBlockChanceChangedDelegate;
	FOnAttributeValueChangedTwoParams MaxSpellBlockChanceChangedDelegate;

protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* InObsidianASC) override;

	/** Character */
	UFUNCTION()
	void HeroLevelUp(const uint8 InNewLevel);
	void ExperienceChanged(const FOnAttributeChangeData& InData) const;
	void MaxExperienceChanged(const FOnAttributeChangeData& InData);
	
	/** Attributes */
	void StrengthChanged(const FOnAttributeChangeData& InData) const;
	void IntelligenceChanged(const FOnAttributeChangeData& InData) const;
	void DexterityChanged(const FOnAttributeChangeData& InData) const;
	void FaithChanged(const FOnAttributeChangeData& InData) const;
	
	/** Vital Attributes */
	void MaxHealthChanged(const FOnAttributeChangeData& InData) const;
	void MaxManaChanged(const FOnAttributeChangeData& InData) const;
	void MaxSpecialResourceChanged(const FOnAttributeChangeData& InData) const;
	void MaxEnergyShieldChanged(const FOnAttributeChangeData& InData) const;
	void MaxStaminaChanged(const FOnAttributeChangeData& InData) const;
	void StaminaRegenerationChanged(const FOnAttributeChangeData& InData) const;
	
	/** Offence */
	void AccuracyChanged(const FOnAttributeChangeData& InData) const;
	void AttackSpeedChanged(const FOnAttributeChangeData& InData) const;
	void CastSpeedChanged(const FOnAttributeChangeData& InData) const;
	void CriticalStrikeChanceChanged(const FOnAttributeChangeData& InData) const;
	void CriticalStrikeDamageMultiplierChanged(const FOnAttributeChangeData& InData) const;
	void PhysicalDamageMultiplierChanged(const FOnAttributeChangeData& InData) const;
	void FireDamageMultiplierChanged(const FOnAttributeChangeData& InData) const;
	void LightningDamageMultiplierChanged(const FOnAttributeChangeData& InData) const;
	void ColdDamageMultiplierChanged(const FOnAttributeChangeData& InData) const;
	void ChaosDamageMultiplierChanged(const FOnAttributeChangeData& InData) const;
	void FirePenetrationChanged(const FOnAttributeChangeData& InData) const;
	void LightningPenetrationChanged(const FOnAttributeChangeData& InData) const;
	void ColdPenetrationChanged(const FOnAttributeChangeData& InData) const;
	void ChaosPenetrationChanged(const FOnAttributeChangeData& InData) const;
	
	/** Defence */
	void ArmorChanged(const FOnAttributeChangeData& InData) const;
	void EvasionChanged(const FOnAttributeChangeData& InData) const;
	void HealthRegenerationChanged(const FOnAttributeChangeData& InData) const;
	void EnergyShieldRegenerationChanged(const FOnAttributeChangeData& InData) const;
	void FireResistanceChanged(const FOnAttributeChangeData& InData) const;
	void MaxFireResistanceChanged(const FOnAttributeChangeData& InData) const;
	void ColdResistanceChanged(const FOnAttributeChangeData& InData) const;
	void MaxColdResistanceChanged(const FOnAttributeChangeData& InData) const;
	void LightningResistanceChanged(const FOnAttributeChangeData& InData) const;
	void MaxLightningResistanceChanged(const FOnAttributeChangeData& InData) const;
	void ChaosResistanceChanged(const FOnAttributeChangeData& InData) const;
	void MaxChaosResistanceChanged(const FOnAttributeChangeData& InData) const;
	void SpellSuppressionChanceChanged(const FOnAttributeChangeData& InData) const;
	void SpellSuppressionMagnitudeChanged(const FOnAttributeChangeData& InData) const;
	void HitBlockChanceChanged(const FOnAttributeChangeData& InData) const;
	void MaxHitBlockChanceChanged(const FOnAttributeChangeData& InData) const;
	void SpellBlockChanceChanged(const FOnAttributeChangeData& InData) const;
	void MaxSpellBlockChanceChanged(const FOnAttributeChangeData& InData) const;
	
protected:
	/** Character */
	FDelegateHandle CharacterLevelChangedDelegateHandle;
	FDelegateHandle ExperienceChangedDelegateHandle;
	FDelegateHandle MaxExperienceChangedDelegateHandle;
	
	FDelegateHandle HeroLevelUpDelegateHandle;
	
	/** Attributes */
	FDelegateHandle StrengthChangedDelegateHandle;
	FDelegateHandle IntelligenceChangedDelegateHandle;
	FDelegateHandle DexterityChangedDelegateHandle;
	FDelegateHandle FaithChangedDelegateHandle;

	/** Vital Attributes */
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle MaxSpecialResourceChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
	FDelegateHandle MaxStaminaChangedDelegateHandle;
	FDelegateHandle StaminaRegenerationChangedDelegateHandle;
	
	/** Offence */
	FDelegateHandle AccuracyChangedDelegateHandle;
	FDelegateHandle AttackSpeedChangedDelegateHandle;
	FDelegateHandle CastSpeedChangedDelegateHandle;
	FDelegateHandle CriticalStrikeChanceChangedDelegateHandle;
	FDelegateHandle CriticalStrikeDamageMultiplierChangedDelegateHandle;
	FDelegateHandle PhysicalDamageMultiplierChangedDelegateHandle;
	FDelegateHandle FireDamageMultiplierChangedDelegateHandle;
	FDelegateHandle LightningDamageMultiplierChangedDelegateHandle;
	FDelegateHandle ColdDamageMultiplierChangedDelegateHandle;
	FDelegateHandle ChaosDamageMultiplierChangedDelegateHandle;
	FDelegateHandle FirePenetrationChangedDelegateHandle;
	FDelegateHandle LightningPenetrationChangedDelegateHandle;
	FDelegateHandle ColdPenetrationChangedDelegateHandle;
	FDelegateHandle ChaosPenetrationChangedDelegateHandle;

	/** Defence */
	FDelegateHandle ArmorChangedDelegateHandle;
	FDelegateHandle EvasionChangedDelegateHandle;
	FDelegateHandle HealthRegenerationChangedDelegateHandle;
	FDelegateHandle EnergyShieldRegenerationChangedDelegateHandle;
	FDelegateHandle FireResistanceChangedDelegateHandle;
	FDelegateHandle MaxFireResistanceChangedDelegateHandle;
	FDelegateHandle LightningResistanceChangedDelegateHandle;
	FDelegateHandle MaxLightningResistanceChangedDelegateHandle;
	FDelegateHandle ColdResistanceChangedDelegateHandle;
	FDelegateHandle MaxColdResistanceChangedDelegateHandle;
	FDelegateHandle ChaosResistanceChangedDelegateHandle;
	FDelegateHandle MaxChaosResistanceChangedDelegateHandle;
	FDelegateHandle SpellSuppressionChanceChangedDelegateHandle;
	FDelegateHandle SpellSuppressionMagnitudeChangedDelegateHandle;
	FDelegateHandle HitBlockChanceChangedDelegateHandle;
	FDelegateHandle MaxHitBlockChanceChangedDelegateHandle;
	FDelegateHandle SpellBlockChanceChangedDelegateHandle;
	FDelegateHandle MaxSpellBlockChanceChangedDelegateHandle;

private:
	UPROPERTY()
	TWeakObjectPtr<UObsidianAbilitySystemComponent> OwnerAbilitySystemComponent = nullptr;
	UPROPERTY()
	TWeakObjectPtr<UObsidianHeroAttributesComponent> OwnerAttributesComponent = nullptr;
	
	float MaxExperienceOldValue = 0.0f;
};

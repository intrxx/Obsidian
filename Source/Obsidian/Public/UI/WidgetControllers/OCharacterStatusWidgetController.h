// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetController.h"
#include "OCharacterStatusWidgetController.generated.h"

DECLARE_DELEGATE_OneParam(FOnAttributeValueChangedOneParam, const float /** Value */);
DECLARE_DELEGATE_TwoParams(FOnAttributeValueChangedTwoParams, const float /** Value 1 */, const float /** Value 2 */);

struct FOnAttributeChangeData;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UOCharacterStatusWidgetController : public UObsidianWidgetController
{
	GENERATED_BODY()

public:
	// ~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	virtual void SetInitialAttributeValues() const override;
	// ~ End of UObsidianWidgetController

public:
	/** Character */
	
	/** Attributes */
	FOnAttributeValueChangedOneParam StrengthValueChangedDelegate;
	FOnAttributeValueChangedOneParam IntelligenceValueChangedDelegate;
	FOnAttributeValueChangedOneParam DexterityValueChangedDelegate;
	FOnAttributeValueChangedOneParam FaithValueChangedDelegate;

	/** Vital Attributes */
	FOnAttributeValueChangedOneParam MaxHealthChangedDelegate;
	FOnAttributeValueChangedOneParam MaxManaChangedDelegate;
	FOnAttributeValueChangedOneParam MaxEnergyShieldChangedDelegate;

	/** Offence */
	FOnAttributeValueChangedOneParam AttackSpeedChangedDelegate;
	FOnAttributeValueChangedOneParam CastSpeedChangedDelegate;
	FOnAttributeValueChangedOneParam CriticalStrikeChanceChangedDelegate;
	FOnAttributeValueChangedOneParam CriticalStrikeMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam PhysicalDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam FireDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam LightningDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam ColdDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam ElementalDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam ChaosDamageMultiplierChangedDelegate;
	FOnAttributeValueChangedOneParam FirePenetrationChangedDelegate;
	FOnAttributeValueChangedOneParam LightningPenetrationChangedDelegate;
	FOnAttributeValueChangedOneParam ColdPenetrationChangedDelegate;
	FOnAttributeValueChangedOneParam ElementalPenetrationChangedDelegate;
	
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

protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC) override;

	/** Character */

	
	/** Attributes */
	void StrengthChanged(const FOnAttributeChangeData& Data) const;
	void IntelligenceChanged(const FOnAttributeChangeData& Data) const;
	void DexterityChanged(const FOnAttributeChangeData& Data) const;
	void FaithChanged(const FOnAttributeChangeData& Data) const;
	
	/** Vital Attributes */
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const;
	
	/** Offence */
	void AttackSpeedChanged(const FOnAttributeChangeData& Data) const;
	void CastSpeedChanged(const FOnAttributeChangeData& Data) const;
	void CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data) const;
	void CriticalStrikeMultiplierChanged(const FOnAttributeChangeData& Data) const;
	void PhysicalDamageMultiplierChanged(const FOnAttributeChangeData& Data) const;
	void FireDamageMultiplierChanged(const FOnAttributeChangeData& Data) const;
	void LightningDamageMultiplierChanged(const FOnAttributeChangeData& Data) const;
	void ColdDamageMultiplierChanged(const FOnAttributeChangeData& Data) const;
	void ElementalDamageMultiplierChanged(const FOnAttributeChangeData& Data) const;
	void ChaosDamageMultiplierChanged(const FOnAttributeChangeData& Data) const;
	void FirePenetrationChanged(const FOnAttributeChangeData& Data) const;
	void LightningPenetrationChanged(const FOnAttributeChangeData& Data) const;
	void ColdPenetrationChanged(const FOnAttributeChangeData& Data) const;
	void ElementalPenetrationChanged(const FOnAttributeChangeData& Data) const;
	
	/** Defence */
	void ArmorChanged(const FOnAttributeChangeData& Data) const;
	void EvasionChanged(const FOnAttributeChangeData& Data) const;
	void HealthRegenerationChanged(const FOnAttributeChangeData& Data) const;
	void EnergyShieldRegenerationChanged(const FOnAttributeChangeData& Data) const;
	void FireResistanceChanged(const FOnAttributeChangeData& Data) const;
	void MaxFireResistanceChanged(const FOnAttributeChangeData& Data) const;
	void ColdResistanceChanged(const FOnAttributeChangeData& Data) const;
	void MaxColdResistanceChanged(const FOnAttributeChangeData& Data) const;
	void LightningResistanceChanged(const FOnAttributeChangeData& Data) const;
	void MaxLightningResistanceChanged(const FOnAttributeChangeData& Data) const;
	void ChaosResistanceChanged(const FOnAttributeChangeData& Data) const;
	void MaxChaosResistanceChanged(const FOnAttributeChangeData& Data) const;
	
protected:
	/** Character */
	FDelegateHandle CharacterLevelChangedDelegateHandle;
	FDelegateHandle ExperienceChangedDelegateHandle;
	FDelegateHandle MaxExperienceChangedDelegateHandle;
	
	/** Attributes */
	FDelegateHandle StrengthChangedDelegateHandle;
	FDelegateHandle IntelligenceChangedDelegateHandle;
	FDelegateHandle DexterityChangedDelegateHandle;
	FDelegateHandle FaithChangedDelegateHandle;

	/** Vital Attributes */
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
	
	/** Offence */
	FDelegateHandle AttackSpeedChangedDelegateHandle;
	FDelegateHandle CastSpeedChangedDelegateHandle;
	FDelegateHandle CriticalStrikeChanceChangedDelegateHandle;
	FDelegateHandle CriticalStrikeMultiplierChangedDelegateHandle;
	FDelegateHandle PhysicalDamageMultiplierChangedDelegateHandle;
	FDelegateHandle FireDamageMultiplierChangedDelegateHandle;
	FDelegateHandle LightningDamageMultiplierChangedDelegateHandle;
	FDelegateHandle ColdDamageMultiplierChangedDelegateHandle;
	FDelegateHandle ElementalDamageMultiplierChangedDelegateHandle;
	FDelegateHandle ChaosDamageMultiplierChangedDelegateHandle;
	FDelegateHandle FirePenetrationChangedDelegateHandle;
	FDelegateHandle LightningPenetrationChangedDelegateHandle;
	FDelegateHandle ColdPenetrationChangedDelegateHandle;
	FDelegateHandle ElementalPenetrationChangedDelegateHandle;

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
};
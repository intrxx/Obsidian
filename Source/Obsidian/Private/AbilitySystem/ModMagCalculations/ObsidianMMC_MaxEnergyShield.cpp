// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxEnergyShield.h"


#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidian_MaxEnergyShield
{
	FGameplayEffectAttributeCaptureDefinition MaxEnergyShield;
	FGameplayEffectAttributeCaptureDefinition Intelligence;
	FGameplayEffectAttributeCaptureDefinition IncreasedEnergyShieldPercentage;
	
	SObsidian_MaxEnergyShield()
	{
		MaxEnergyShield = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		Intelligence = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetIntelligenceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		IncreasedEnergyShieldPercentage = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetIncreasedHealthPercentageAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_MaxEnergyShield& MaxEnergyShieldStatics()
{
	static const SObsidian_MaxEnergyShield MaxEnergyShield;
	return MaxEnergyShield;
}

UObsidianMMC_MaxEnergyShield::UObsidianMMC_MaxEnergyShield(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(MaxEnergyShieldStatics().MaxEnergyShield);
	RelevantAttributesToCapture.Add(MaxEnergyShieldStatics().Intelligence);
	RelevantAttributesToCapture.Add(MaxEnergyShieldStatics().IncreasedEnergyShieldPercentage);
}

float UObsidianMMC_MaxEnergyShield::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FAggregatorEvaluateParameters EvaluationParameters;
	
	float MaxEnergyShield = 0.f;
	GetCapturedAttributeMagnitude(MaxEnergyShieldStatics().MaxEnergyShield, Spec, EvaluationParameters, MaxEnergyShield);
	MaxEnergyShield = FMath::Max<float>(MaxEnergyShield, 0.f);

	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(MaxEnergyShieldStatics().Intelligence, Spec, EvaluationParameters, Intelligence);
	Intelligence = FMath::Max<float>(Intelligence, 0.f);

	float IncreasedEnergyShieldPercentage = 0.f;
	GetCapturedAttributeMagnitude(MaxEnergyShieldStatics().IncreasedEnergyShieldPercentage, Spec, EvaluationParameters, IncreasedEnergyShieldPercentage);
	IncreasedEnergyShieldPercentage = FMath::Max<float>(IncreasedEnergyShieldPercentage, 0.f);

	const float EnergyShieldMagnitude = FMath::FloorToInt((MaxEnergyShield * IncreasedEnergyShieldPercentage / 100.0f));
	const float EnergyShieldBonus = (3 * Intelligence);
	const float FinalEnergyShield = EnergyShieldMagnitude + EnergyShieldBonus;
	return FinalEnergyShield;
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxChaosResistance.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidian_MaxChaosResistanceStatics
{
	FGameplayEffectAttributeCaptureDefinition MaxChaosResistanceDef;
	FGameplayEffectAttributeCaptureDefinition FaithDef;
	
	SObsidian_MaxChaosResistanceStatics()
	{
		MaxChaosResistanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMaxChaosResistanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
		FaithDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetFaithAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_MaxChaosResistanceStatics& MaxChaosResistanceStatics()
{
	static const SObsidian_MaxChaosResistanceStatics MaxChaosResistanceStatics;
	return MaxChaosResistanceStatics;
}

UObsidianMMC_MaxChaosResistance::UObsidianMMC_MaxChaosResistance()
{
	RelevantAttributesToCapture.Add(MaxChaosResistanceStatics().MaxChaosResistanceDef);
	RelevantAttributesToCapture.Add(MaxChaosResistanceStatics().FaithDef);
}

float UObsidianMMC_MaxChaosResistance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float MaxChaosResistance = 0.f;
	GetCapturedAttributeMagnitude(MaxChaosResistanceStatics().MaxChaosResistanceDef, Spec, EvaluationParameters, MaxChaosResistance);
	MaxChaosResistance = FMath::Max<float>(MaxChaosResistance, 0.f);

	float Faith = 0.f;
	GetCapturedAttributeMagnitude(MaxChaosResistanceStatics().FaithDef, Spec, EvaluationParameters, Faith);
	Faith = FMath::Max<float>(Faith, 0.f);

	const float NewMaxChaosResistance = MaxChaosResistance + FMath::FloorToInt(Faith / 3);
	return NewMaxChaosResistance;
}

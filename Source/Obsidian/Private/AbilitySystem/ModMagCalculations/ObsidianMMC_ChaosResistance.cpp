// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/ModMagCalculations/ObsidianMMC_ChaosResistance.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidian_ChaosResistanceStatics
{
	FGameplayEffectAttributeCaptureDefinition FaithDef;
	
	SObsidian_ChaosResistanceStatics()
	{
		FaithDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetFaithAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_ChaosResistanceStatics& ChaosResistanceStatics()
{
	static const SObsidian_ChaosResistanceStatics ChaosResistanceStatics;
	return ChaosResistanceStatics;
}

UObsidianMMC_ChaosResistance::UObsidianMMC_ChaosResistance()
{
	RelevantAttributesToCapture.Add(ChaosResistanceStatics().FaithDef);
}

float UObsidianMMC_ChaosResistance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float Faith = 0.f;
	GetCapturedAttributeMagnitude(ChaosResistanceStatics().FaithDef, Spec, EvaluationParameters, Faith);
	Faith = FMath::Max<float>(Faith, 0.f);

	const float ChaosResistanceBonus = FMath::FloorToInt(Faith / 3);
	return ChaosResistanceBonus;
}

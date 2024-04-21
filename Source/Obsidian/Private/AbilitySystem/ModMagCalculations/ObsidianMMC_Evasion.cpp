// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/ModMagCalculations/ObsidianMMC_Evasion.h"

#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidian_EvasionStatics
{
	FGameplayEffectAttributeCaptureDefinition EvasionDef;
	FGameplayEffectAttributeCaptureDefinition DexterityDef;

	SObsidian_EvasionStatics()
	{
		EvasionDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetEvasionAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetDexterityAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_EvasionStatics& EvasionStatics()
{
	static const SObsidian_EvasionStatics EvasionStatics;
	return EvasionStatics;
}

UObsidianMMC_Evasion::UObsidianMMC_Evasion()
{
	RelevantAttributesToCapture.Add(EvasionStatics().EvasionDef);
	RelevantAttributesToCapture.Add(EvasionStatics().DexterityDef);
}

float UObsidianMMC_Evasion::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Evasion = 0.f;
	GetCapturedAttributeMagnitude(EvasionStatics().EvasionDef, Spec, EvaluationParameters, Evasion);
	Evasion = FMath::Max<float>(Evasion, 0.f);

	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(EvasionStatics().DexterityDef, Spec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);

	const float NewEvasion = Evasion + (FMath::FloorToInt(Dexterity / 2) * 3);
	return NewEvasion;
}

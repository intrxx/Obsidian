// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_Evasion.h"


#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidian_EvasionStatics
{
	FGameplayEffectAttributeCaptureDefinition Evasion;
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	FGameplayEffectAttributeCaptureDefinition IncreasedEvasionPercentage;

	SObsidian_EvasionStatics()
	{
		Evasion = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetEvasionAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetDexterityAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		IncreasedEvasionPercentage = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetIncreasedEvasionPercentAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_EvasionStatics& EvasionStatics()
{
	static const SObsidian_EvasionStatics EvasionStatics;
	return EvasionStatics;
}

UObsidianMMC_Evasion::UObsidianMMC_Evasion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(EvasionStatics().Evasion);
	RelevantAttributesToCapture.Add(EvasionStatics().DexterityDef);
	RelevantAttributesToCapture.Add(EvasionStatics().IncreasedEvasionPercentage);
}

float UObsidianMMC_Evasion::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FAggregatorEvaluateParameters EvaluationParameters;
	
	float Evasion = 0.f;
	GetCapturedAttributeMagnitude(EvasionStatics().Evasion, Spec, EvaluationParameters, Evasion);
	Evasion = FMath::Max<float>(Evasion, 0.f);
	
	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(EvasionStatics().DexterityDef, Spec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);

	float IncreasedEvasionPercentage = 0.f;
	GetCapturedAttributeMagnitude(EvasionStatics().IncreasedEvasionPercentage, Spec, EvaluationParameters, IncreasedEvasionPercentage);
	IncreasedEvasionPercentage = FMath::Max<float>(IncreasedEvasionPercentage, 0.f);

	const float EvasionMagnitude = FMath::FloorToInt((Evasion * IncreasedEvasionPercentage / 100.0f));
	const float EvasionBonus = (FMath::FloorToInt(Dexterity / 2) * 3);
	const float FinalEvasion = EvasionMagnitude + EvasionBonus;
	return FinalEvasion;
}

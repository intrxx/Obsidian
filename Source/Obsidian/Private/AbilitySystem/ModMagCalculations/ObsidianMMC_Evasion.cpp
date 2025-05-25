// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_Evasion.h"

// ~ Core


// ~ Project
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidian_EvasionStatics
{
	FGameplayEffectAttributeCaptureDefinition DexterityDef;

	SObsidian_EvasionStatics()
	{
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
	RelevantAttributesToCapture.Add(EvasionStatics().DexterityDef);
}

float UObsidianMMC_Evasion::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(EvasionStatics().DexterityDef, Spec, FAggregatorEvaluateParameters(), Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);

	const float EvasionBonus = (FMath::FloorToInt(Dexterity / 2) * 3);
	return EvasionBonus;
}

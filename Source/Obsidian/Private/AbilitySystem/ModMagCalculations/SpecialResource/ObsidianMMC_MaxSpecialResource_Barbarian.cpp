// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource_Barbarian.h"

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidianMMC_MaxSpecialResource_Barbarian
{
	FGameplayEffectAttributeCaptureDefinition StrengthDef;

	SObsidianMMC_MaxSpecialResource_Barbarian()
	{
		StrengthDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetStrengthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidianMMC_MaxSpecialResource_Barbarian& MaxSpecialResource_BarbarianStatics()
{
	static const SObsidianMMC_MaxSpecialResource_Barbarian MaxSpecialResource_WitchStatics;
	return MaxSpecialResource_WitchStatics;
}

UObsidianMMC_MaxSpecialResource_Barbarian::UObsidianMMC_MaxSpecialResource_Barbarian()
{
}

float UObsidianMMC_MaxSpecialResource_Barbarian::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// OBS-79
	// const float LevelAddedSpecialResource = Super::CalculateBaseMagnitude_Implementation(Spec);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(MaxSpecialResource_BarbarianStatics().StrengthDef, Spec, FAggregatorEvaluateParameters(), Strength);
	Strength = FMath::Max<float>(Strength, 0.f);

	const float MaxSpecialResourceBonus_Witch = /** OBS-79 / LevelAddedSpecialResource + */ FMath::FloorToInt(Strength / 6);
	return MaxSpecialResourceBonus_Witch;
}

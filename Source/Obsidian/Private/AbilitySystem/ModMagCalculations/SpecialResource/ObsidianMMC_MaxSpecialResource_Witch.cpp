// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource_Witch.h"

// ~ Core

// ~ Project
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidian_MaxSpecialResource_Witch
{
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;

	SObsidian_MaxSpecialResource_Witch()
	{
		IntelligenceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetIntelligenceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_MaxSpecialResource_Witch& MaxSpecialResource_WitchStatics()
{
	static const SObsidian_MaxSpecialResource_Witch MaxSpecialResource_WitchStatics;
	return MaxSpecialResource_WitchStatics;
}

UObsidianMMC_MaxSpecialResource_Witch::UObsidianMMC_MaxSpecialResource_Witch()
{
	RelevantAttributesToCapture.Add(MaxSpecialResource_WitchStatics().IntelligenceDef);
}

float UObsidianMMC_MaxSpecialResource_Witch::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// OBS-79
	// const float LevelAddedSpecialResource = Super::CalculateBaseMagnitude_Implementation(Spec);
	
	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(MaxSpecialResource_WitchStatics().IntelligenceDef, Spec, FAggregatorEvaluateParameters(), Intelligence);
	Intelligence = FMath::Max<float>(Intelligence, 0.f);

	const float MaxSpecialResourceBonus_Witch = /** OBS-79 / LevelAddedSpecialResource  + */ FMath::FloorToInt(Intelligence / 6);
	return MaxSpecialResourceBonus_Witch;
}

// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource_Paladin.h"

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidianMMC_MaxSpecialResource_Paladin
{
	FGameplayEffectAttributeCaptureDefinition FaithDef;

	SObsidianMMC_MaxSpecialResource_Paladin()
	{
		FaithDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetFaithAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidianMMC_MaxSpecialResource_Paladin& MaxSpecialResource_PaladinStatics()
{
	static const SObsidianMMC_MaxSpecialResource_Paladin MaxSpecialResource_WitchStatics;
	return MaxSpecialResource_WitchStatics;
}

UObsidianMMC_MaxSpecialResource_Paladin::UObsidianMMC_MaxSpecialResource_Paladin()
{
	RelevantAttributesToCapture.Add(MaxSpecialResource_PaladinStatics().FaithDef);
}

float UObsidianMMC_MaxSpecialResource_Paladin::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// OBS-79
	// const float LevelAddedSpecialResource = Super::CalculateBaseMagnitude_Implementation(Spec);
	
	float Faith = 0.f;
	GetCapturedAttributeMagnitude(MaxSpecialResource_PaladinStatics().FaithDef, Spec, FAggregatorEvaluateParameters(), Faith);
	Faith = FMath::Max<float>(Faith, 0.f);

	const float MaxSpecialResourceBonus_Witch = /** OBS-79 / LevelAddedSpecialResource + */ FMath::FloorToInt(Faith / 5);
	return MaxSpecialResourceBonus_Witch;
}

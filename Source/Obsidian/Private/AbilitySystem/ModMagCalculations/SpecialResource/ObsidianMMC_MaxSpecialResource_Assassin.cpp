// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource_Assassin.h"


#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidianMMC_MaxSpecialResource_Assassin
{
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;

	SObsidianMMC_MaxSpecialResource_Assassin()
	{
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetDexterityAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		IntelligenceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetIntelligenceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidianMMC_MaxSpecialResource_Assassin& MaxSpecialResource_AssassinStatics()
{
	static const SObsidianMMC_MaxSpecialResource_Assassin MaxSpecialResource_WitchStatics;
	return MaxSpecialResource_WitchStatics;
}

UObsidianMMC_MaxSpecialResource_Assassin::UObsidianMMC_MaxSpecialResource_Assassin(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(MaxSpecialResource_AssassinStatics().DexterityDef);
	RelevantAttributesToCapture.Add(MaxSpecialResource_AssassinStatics().IntelligenceDef);
}

float UObsidianMMC_MaxSpecialResource_Assassin::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// OBS-79
	// const float LevelAddedSpecialResource = Super::CalculateBaseMagnitude_Implementation(Spec);

	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(MaxSpecialResource_AssassinStatics().IntelligenceDef, Spec, FAggregatorEvaluateParameters(), Intelligence);
	Intelligence = FMath::Max<float>(Intelligence, 0.f);
	
	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(MaxSpecialResource_AssassinStatics().DexterityDef, Spec, FAggregatorEvaluateParameters(), Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);

	const float MaxSpecialResourceBonus_Witch = /** OBS-79 / LevelAddedSpecialResource + */ FMath::FloorToInt(Intelligence / 10) + FMath::FloorToInt(Dexterity / 10);
	return MaxSpecialResourceBonus_Witch;
}

// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_Accuracy.h"


#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidian_AccuracyStatics
{
	FGameplayEffectAttributeCaptureDefinition DexterityDef;

	SObsidian_AccuracyStatics()
	{
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetDexterityAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_AccuracyStatics& AccuracyStatics()
{
	static const SObsidian_AccuracyStatics AccuracyStatics;
	return AccuracyStatics;
}

UObsidianMMC_Accuracy::UObsidianMMC_Accuracy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(AccuracyStatics().DexterityDef);
}

float UObsidianMMC_Accuracy::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(AccuracyStatics().DexterityDef, Spec, FAggregatorEvaluateParameters(), Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);

	const float DexterityBonus = (FMath::FloorToInt(Dexterity / 2) * 5);
	return DexterityBonus;
}

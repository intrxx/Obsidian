// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_Armor.h"

#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

struct SObsidian_ArmorStatics
{
	FGameplayEffectAttributeCaptureDefinition Armor;
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition IncreasedArmorPercentage;

	SObsidian_ArmorStatics()
	{
		Armor = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetArmorAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		StrengthDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetStrengthAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		IncreasedArmorPercentage = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetIncreasedArmorPercentAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_ArmorStatics& ArmorStatics()
{
	static const SObsidian_ArmorStatics ArmorStatics;
	return ArmorStatics;
}

UObsidianMMC_Armor::UObsidianMMC_Armor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(ArmorStatics().Armor);
	RelevantAttributesToCapture.Add(ArmorStatics().StrengthDef);
	RelevantAttributesToCapture.Add(ArmorStatics().IncreasedArmorPercentage);
}

float UObsidianMMC_Armor::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FAggregatorEvaluateParameters EvaluationParameters;
	
	float Armor = 0.f;
	GetCapturedAttributeMagnitude(ArmorStatics().Armor, Spec, EvaluationParameters, Armor);
	Armor = FMath::Max<float>(Armor, 0.f);
	
	float Strength = 0.f;
	GetCapturedAttributeMagnitude(ArmorStatics().StrengthDef, Spec, EvaluationParameters, Strength);
	Strength = FMath::Max<float>(Strength, 0.f);

	float IncreasedArmorPercentage = 0.f;
	GetCapturedAttributeMagnitude(ArmorStatics().IncreasedArmorPercentage, Spec, EvaluationParameters, IncreasedArmorPercentage);
	IncreasedArmorPercentage = FMath::Max<float>(IncreasedArmorPercentage, 0.f);

	const float ArmorIncreaseMagnitude = FMath::FloorToInt((Armor * IncreasedArmorPercentage / 100.0f));
	const float ArmorBonus = FMath::FloorToInt(Strength / 4) + ArmorIncreaseMagnitude;
	return ArmorBonus;
}

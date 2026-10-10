// Copyright 2026 out of sCope - intrxx

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxStamina.h"

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Combat/ObsidianCombatInterface.h"
#include "Obsidian/ObsidianLogCategories.h"


struct SObsidian_MaxStaminaStatics
{
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	
	SObsidian_MaxStaminaStatics()
	{
		StrengthDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetStrengthAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetDexterityAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_MaxStaminaStatics& MaxStaminaStatics()
{
	static const SObsidian_MaxStaminaStatics MaxStaminaStatics;
	return MaxStaminaStatics;
}

UObsidianMMC_MaxStamina::UObsidianMMC_MaxStamina(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	RelevantAttributesToCapture.Add(MaxStaminaStatics().StrengthDef);
	RelevantAttributesToCapture.Add(MaxStaminaStatics().DexterityDef);
}

float UObsidianMMC_MaxStamina::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& InSpec) const
{
	IObsidianCombatInterface* CombatInterface = Cast<IObsidianCombatInterface>(InSpec.GetContext().GetSourceObject());
	if (CombatInterface == nullptr)
	{
		UE_LOG(ObLogAbilitySystem, Error, TEXT("Combat Interface on [%s] is null, please double check the Source Object"),
			*GetNameSafe(this));
		return Super::CalculateBaseMagnitude_Implementation(InSpec);
	}
	
	const uint8 CharacterLevel = CombatInterface->GetCharacterLevel();
	float LevelAddedMaxStaminaValue = 0.f;
	if(MaxStaminaBonusCurveTable)
	{
		if(const FRealCurve* Curve = MaxStaminaBonusCurveTable->FindCurve(FName("MaxStamina"), FString("")))
		{
			LevelAddedMaxStaminaValue = Curve->Eval(CharacterLevel);
		}
	}
	
	const FAggregatorEvaluateParameters EvaluationParameters;

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(MaxStaminaStatics().StrengthDef, InSpec, EvaluationParameters, Strength);
	Strength = FMath::Max<float>(Strength, 0.f);
	
	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(MaxStaminaStatics().DexterityDef, InSpec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);
	
	const float MaxManaBonus = Dexterity * 2 + Strength + LevelAddedMaxStaminaValue;
	return MaxManaBonus;
}

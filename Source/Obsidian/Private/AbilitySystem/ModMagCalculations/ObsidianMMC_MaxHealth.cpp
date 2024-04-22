// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxHealth.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Engine/CurveTable.h"
#include "Combat/ObsidianCombatInterface.h"

struct SObsidian_MaxHealthStatics
{
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	
	SObsidian_MaxHealthStatics()
	{
		MaxHealthDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
		StrengthDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetStrengthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetDexterityAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_MaxHealthStatics& MaxHealthStatics()
{
	static const SObsidian_MaxHealthStatics MaxHealthStatics;
	return MaxHealthStatics;
}

UObsidianMMC_MaxHealth::UObsidianMMC_MaxHealth()
{
	RelevantAttributesToCapture.Add(MaxHealthStatics().MaxHealthDef);
	RelevantAttributesToCapture.Add(MaxHealthStatics().StrengthDef);
	RelevantAttributesToCapture.Add(MaxHealthStatics().DexterityDef);
}

float UObsidianMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// We can get the combat interface because the source object is our Character that executes the backing attributes infinite gameplay effect
	IObsidianCombatInterface* CombatInterface = Cast<IObsidianCombatInterface>(Spec.GetContext().GetSourceObject());
	if (CombatInterface == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Combat Interface on [%s] is null, please double check the Source Object"), *GetNameSafe(this));
		return Super::CalculateBaseMagnitude_Implementation(Spec);
	}
	
	const int32 CharacterLevel = CombatInterface->GetCharacterLevel();
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	float MaxHealth = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().MaxHealthDef, Spec, EvaluationParameters, MaxHealth);
	MaxHealth = FMath::Max<float>(MaxHealth, 0.f);

	float Strength = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().StrengthDef, Spec, EvaluationParameters, Strength);
	Strength = FMath::Max<float>(Strength, 0.f);

	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().DexterityDef, Spec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);
	
	float LevelAddedHealthValue = 0.f;
	if(HealthAwardCurveTable)
	{
		const FRealCurve* Curve = HealthAwardCurveTable->FindCurve(FName("Curve"), FString(""));
		if(Curve)
		{
			LevelAddedHealthValue = Curve->Eval(CharacterLevel);
		}
	}
	
	const float NewMaxHealth = MaxHealth + Dexterity + (2 * Strength) + (CharacterLevel * LevelAddedHealthValue);
	return NewMaxHealth;
}

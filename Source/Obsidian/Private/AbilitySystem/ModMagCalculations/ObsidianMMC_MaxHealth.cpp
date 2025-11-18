// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxHealth.h"

#include <Engine/CurveTable.h>

#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Combat/ObsidianCombatInterface.h"
#include "Obsidian/ObsidianGameModule.h"

struct SObsidian_MaxHealthStatics
{
	FGameplayEffectAttributeCaptureDefinition MaxHealth;
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	FGameplayEffectAttributeCaptureDefinition IncreasedHealthPercentage;
	
	SObsidian_MaxHealthStatics()
	{
		MaxHealth = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		StrengthDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetStrengthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetDexterityAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		IncreasedHealthPercentage = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetIncreasedHealthPercentageAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_MaxHealthStatics& MaxHealthStatics()
{
	static const SObsidian_MaxHealthStatics MaxHealthStatics;
	return MaxHealthStatics;
}

UObsidianMMC_MaxHealth::UObsidianMMC_MaxHealth(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(MaxHealthStatics().MaxHealth);
	RelevantAttributesToCapture.Add(MaxHealthStatics().StrengthDef);
	RelevantAttributesToCapture.Add(MaxHealthStatics().DexterityDef);
	RelevantAttributesToCapture.Add(MaxHealthStatics().IncreasedHealthPercentage);
}

float UObsidianMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// We can get the combat interface because the source object is our Character that executes the backing attributes infinite gameplay effect
	IObsidianCombatInterface* CombatInterface = Cast<IObsidianCombatInterface>(Spec.GetContext().GetSourceObject());
	if (CombatInterface == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Combat Interface on [%s] is null, please double check the Source Object"), *GetNameSafe(this));
		return Super::CalculateBaseMagnitude_Implementation(Spec);
	}
	
	const uint8 CharacterLevel = CombatInterface->GetCharacterLevel();
	
	const FAggregatorEvaluateParameters EvaluationParameters;

	float MaxHealth = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().MaxHealth, Spec, EvaluationParameters, MaxHealth);
	MaxHealth = FMath::Max<float>(MaxHealth, 0.f);
	
	float Strength = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().StrengthDef, Spec, EvaluationParameters, Strength);
	Strength = FMath::Max<float>(Strength, 0.f);

	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().DexterityDef, Spec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);

	float IncreasedHealth = 0.f;
	GetCapturedAttributeMagnitude(MaxHealthStatics().IncreasedHealthPercentage, Spec, EvaluationParameters, IncreasedHealth);
	IncreasedHealth = FMath::Max<float>(IncreasedHealth, 0.f);

	//TICKET(intrxx) OBS-79
	// float LevelAddedMaxHealthValue = 0.f;
	// if(MaxHealthAwardCurveTable)
	// {
	// 	if(const FRealCurve* Curve = MaxHealthAwardCurveTable->FindCurve(FName("MaxHealth"), FString("")))
	// 	{
	// 		LevelAddedMaxHealthValue = Curve->Eval(CharacterLevel);
	// 	}
	// }

	const float HealthIncreaseMagnitude = FMath::FloorToInt((MaxHealth * IncreasedHealth / 100.0f));
	const float MaxHealthBonus = Dexterity + (2 * Strength) /**TICKET(intrxx) OBS-79 / + LevelAddedSpecialResource */ + HealthIncreaseMagnitude;
	return MaxHealthBonus;
}

// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxMana.h"

#include <Engine/CurveTable.h>

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Combat/ObsidianCombatInterface.h"
#include "Obsidian/ObsidianGameModule.h"

struct SObsidian_MaxManaStatics
{
	FGameplayEffectAttributeCaptureDefinition MaxMana;
	FGameplayEffectAttributeCaptureDefinition FaithDef;
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	FGameplayEffectAttributeCaptureDefinition IncreasedManaPercentage;
	
	SObsidian_MaxManaStatics()
	{
		MaxMana = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetMaxManaAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		FaithDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetFaithAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetDexterityAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		IncreasedManaPercentage = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetIncreasedManaPercentageAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_MaxManaStatics& MaxManaStatics()
{
	static const SObsidian_MaxManaStatics MaxManaStatics;
	return MaxManaStatics;
}

UObsidianMMC_MaxMana::UObsidianMMC_MaxMana(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(MaxManaStatics().MaxMana);
	RelevantAttributesToCapture.Add(MaxManaStatics().FaithDef);
	RelevantAttributesToCapture.Add(MaxManaStatics().DexterityDef);
	RelevantAttributesToCapture.Add(MaxManaStatics().IncreasedManaPercentage);
}

float UObsidianMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	IObsidianCombatInterface* CombatInterface = Cast<IObsidianCombatInterface>(Spec.GetContext().GetSourceObject());
	if (CombatInterface == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Combat Interface on [%s] is null, please double check the Source Object"), *GetNameSafe(this));
		return Super::CalculateBaseMagnitude_Implementation(Spec);
	}
	
	const uint8 CharacterLevel = CombatInterface->GetCharacterLevel();
	
	const FAggregatorEvaluateParameters EvaluationParameters;

	float MaxMana = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().MaxMana, Spec, EvaluationParameters, MaxMana);
	MaxMana = FMath::Max<float>(MaxMana, 0.f);
	
	float Faith = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().FaithDef, Spec, EvaluationParameters, Faith);
	Faith = FMath::Max<float>(Faith, 0.f);

	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().DexterityDef, Spec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);

	float IncreasedManaPercentage = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().IncreasedManaPercentage, Spec, EvaluationParameters, IncreasedManaPercentage);
	IncreasedManaPercentage = FMath::Max<float>(IncreasedManaPercentage, 0.f);
	
	// OBS-79
	// float LevelAddedMaxManaValue = 0.f;
	// if(MaxManaAwardCurveTable)
	// {
	// 	if(const FRealCurve* Curve = MaxManaAwardCurveTable->FindCurve(FName("MaxMana"), FString("")))
	// 	{
	// 		LevelAddedMaxManaValue = Curve->Eval(CharacterLevel);
	// 	}
	// }

	const float ManaIncreaseMagnitude = FMath::FloorToInt((MaxMana * IncreasedManaPercentage / 100.0f));
	const float MaxManaBonus = Dexterity + (2 * Faith) /** OBS-79 / + LevelAddedSpecialResource */ + ManaIncreaseMagnitude;
	return MaxManaBonus;
}

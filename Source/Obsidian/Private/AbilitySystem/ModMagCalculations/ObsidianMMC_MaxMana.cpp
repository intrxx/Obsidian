// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxMana.h"

// ~ Core
#include "Engine/CurveTable.h"

// ~ Project
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Combat/ObsidianCombatInterface.h"
#include "Obsidian/ObsidianGameModule.h"

struct SObsidian_MaxManaStatics
{
	FGameplayEffectAttributeCaptureDefinition FaithDef;
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	
	SObsidian_MaxManaStatics()
	{
		FaithDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetFaithAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		DexterityDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetDexterityAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const SObsidian_MaxManaStatics& MaxManaStatics()
{
	static const SObsidian_MaxManaStatics MaxManaStatics;
	return MaxManaStatics;
}

UObsidianMMC_MaxMana::UObsidianMMC_MaxMana()
{
	RelevantAttributesToCapture.Add(MaxManaStatics().FaithDef);
	RelevantAttributesToCapture.Add(MaxManaStatics().DexterityDef);
}

float UObsidianMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	IObsidianCombatInterface* CombatInterface = Cast<IObsidianCombatInterface>(Spec.GetContext().GetSourceObject());
	if (CombatInterface == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Combat Interface on [%s] is null, please double check the Source Object"), *GetNameSafe(this));
		return Super::CalculateBaseMagnitude_Implementation(Spec);
	}
	
	const int32 CharacterLevel = CombatInterface->GetCharacterLevel();
	
	const FAggregatorEvaluateParameters EvaluationParameters;
	
	float Faith = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().FaithDef, Spec, EvaluationParameters, Faith);
	Faith = FMath::Max<float>(Faith, 0.f);

	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().DexterityDef, Spec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);

	// OBS-79
	// float LevelAddedMaxManaValue = 0.f;
	// if(MaxManaAwardCurveTable)
	// {
	// 	if(const FRealCurve* Curve = MaxManaAwardCurveTable->FindCurve(FName("MaxMana"), FString("")))
	// 	{
	// 		LevelAddedMaxManaValue = Curve->Eval(CharacterLevel);
	// 	}
	// }
	
	const float MaxManaBonus = Dexterity + (2 * Faith) /** OBS-79 / + LevelAddedSpecialResource */;
	return MaxManaBonus;
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxMana.h"
#include "Engine/CurveTable.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Combat/ObsidianCombatInterface.h"

struct SObsidian_MaxManaStatics
{
	FGameplayEffectAttributeCaptureDefinition MaxManaDef;
	FGameplayEffectAttributeCaptureDefinition FaithDef;
	FGameplayEffectAttributeCaptureDefinition DexterityDef;
	
	SObsidian_MaxManaStatics()
	{
		MaxManaDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetMaxManaAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
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
	RelevantAttributesToCapture.Add(MaxManaStatics().MaxManaDef);
	RelevantAttributesToCapture.Add(MaxManaStatics().FaithDef);
	RelevantAttributesToCapture.Add(MaxManaStatics().DexterityDef);
}

float UObsidianMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
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

	float MaxMana = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().MaxManaDef, Spec, EvaluationParameters, MaxMana);
	MaxMana = FMath::Max<float>(MaxMana, 0.f);

	float Faith = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().FaithDef, Spec, EvaluationParameters, Faith);
	Faith = FMath::Max<float>(Faith, 0.f);

	float Dexterity = 0.f;
	GetCapturedAttributeMagnitude(MaxManaStatics().DexterityDef, Spec, EvaluationParameters, Dexterity);
	Dexterity = FMath::Max<float>(Dexterity, 0.f);
	
	float LevelAddedManaValue = 0.f;
	if(ManaAwardCurveTable)
	{
		const FRealCurve* Curve = ManaAwardCurveTable->FindCurve(FName("Curve"), FString(""));
		if(Curve)
		{
			LevelAddedManaValue = Curve->Eval(CharacterLevel);
		}
	}
	
	const float NewMaxMana = MaxMana + Dexterity + (2 * Faith) + (CharacterLevel * LevelAddedManaValue);
	return NewMaxMana;
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Executions/ObsidianDamageExecution.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Obsidian/ObsidianGameplayTags.h"

struct FObsidianDamageStatics
{
	// Source
	FGameplayEffectAttributeCaptureDefinition AccuracyDef;
	FGameplayEffectAttributeCaptureDefinition CriticalStrikeChanceDef;
	FGameplayEffectAttributeCaptureDefinition CriticalStrikeDamageMultiplierDef;
	
	// Target
	FGameplayEffectAttributeCaptureDefinition EvasionDef;
	FGameplayEffectAttributeCaptureDefinition ArmorDef;
	FGameplayEffectAttributeCaptureDefinition SpellSuppressionChanceDef;
	FGameplayEffectAttributeCaptureDefinition SpellSuppressionMagnitudeDef;

	FObsidianDamageStatics()
	{
		// Source
		AccuracyDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetAccuracyAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		CriticalStrikeChanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetCriticalStrikeChanceAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		CriticalStrikeDamageMultiplierDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetCriticalStrikeDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		
		// Target
		EvasionDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetEvasionAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		ArmorDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetArmorAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		SpellSuppressionChanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetSpellSuppressionChanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		SpellSuppressionMagnitudeDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetSpellSuppressionMagnitudeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const FObsidianDamageStatics& ObsidianDamageStatics()
{
	static FObsidianDamageStatics ObsidianDamageStatics;
	return ObsidianDamageStatics;
}

UObsidianDamageExecution::UObsidianDamageExecution()
{
	// Source
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().AccuracyDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().CriticalStrikeChanceDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().CriticalStrikeDamageMultiplierDef);
	
	// Target
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().EvasionDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().SpellSuppressionChanceDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().SpellSuppressionMagnitudeDef);
}

void UObsidianDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	const float FullDamage = Spec.GetSetByCallerMagnitude(ObsidianGameplayTags::SetByCaller_Damage);
	float ModifiedDamage = FullDamage;

	//TODO Only evade Hits
	// ~ Start of Hit Evasion Calculation
	float Evasion = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().EvasionDef, EvaluationParameters, Evasion);
	Evasion = FMath::Max<float>(Evasion, 0.0f);

	float Accuracy = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().AccuracyDef, EvaluationParameters, Accuracy);
	Accuracy = FMath::Max<float>(Accuracy, 0.0f);

	//TODO Read about entropy hit here: https://pathofexile.fandom.com/wiki/Evasion#cite_note-cite4-2, https://www.pathofexile.com/forum/view-thread/11707#p216024 and maybe implement it
	float ChanceToHit = ((Accuracy * 1.25f) / ((Accuracy + FMath::Pow((Evasion * 0.25f), 0.8f)))) * 100.0f;
	ChanceToHit = FMath::Clamp<float>(ChanceToHit, 5.0f, 100.0f);
	
	if(!(ChanceToHit >= FMath::RandRange(1.0f, 100.0f))) // We did not hit return 0 damage
	{
		const FGameplayModifierEvaluatedData& ModifierEvaluatedData = FGameplayModifierEvaluatedData(UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, 0.0f);
		OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
		
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("The hit was evaded. Chance to hit was: %f. New damage: 0"), ChanceToHit));
#endif
		
		return;
	}
	// ~ End of Evasion Calculation Hit

	// ~ Start of Critical Strikes Calculation
	float CriticalStrikeChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().CriticalStrikeChanceDef, EvaluationParameters, CriticalStrikeChance);
	CriticalStrikeChance = FMath::Max<float>(CriticalStrikeChance, 0.0f);

	float CriticalStrikeDamageMultiplier = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().CriticalStrikeDamageMultiplierDef, EvaluationParameters, CriticalStrikeDamageMultiplier);
	CriticalStrikeDamageMultiplier = FMath::Max<float>(CriticalStrikeDamageMultiplier, 0.0f);

	if(CriticalStrikeChance >= FMath::RandRange(1.0f, 100.0f))
	{
		ModifiedDamage = ModifiedDamage * (CriticalStrikeDamageMultiplier / 100.0f);
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
			FString::Printf(TEXT("Critical Strike! New damage: %f."), ModifiedDamage));
#endif
	}
	// ~ End of Critical Strikes Calculation

	//TODO Only mitigate raw physical damage
	// ~ Start of Armor Raw Physical Damage Mitigation
	float Armor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().ArmorDef, EvaluationParameters, Armor);
	Armor = FMath::Max<float>(Armor, 0.0f);
	
	const float RawPhysicalDamageMitigation = Armor / (Armor + 5 * ModifiedDamage);
	ModifiedDamage -= RawPhysicalDamageMitigation;
	
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
		FString::Printf(TEXT("Reducing raw physical damage. Damage reduced: %f. New damage: %f."), RawPhysicalDamageMitigation, ModifiedDamage));
#endif
	// ~ End of Armor Raw Physical Damage Mitigation

	//TODO Only attempt to suppress spell damage
	// ~ Start of Suppression Spell Damage Mitigation
	float SpellSuppressionChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().SpellSuppressionChanceDef, EvaluationParameters, SpellSuppressionChance);
	SpellSuppressionChance = FMath::Max<float>(SpellSuppressionChance, 0.0f);
	
	if(SpellSuppressionChance >= FMath::RandRange(1.0f, 100.0f))
	{
		float SpellSuppressionMagnitude = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().SpellSuppressionMagnitudeDef, EvaluationParameters, SpellSuppressionMagnitude);
		SpellSuppressionMagnitude = FMath::Max<float>(SpellSuppressionMagnitude, 0.0f);

		const float SpellSuppressionMitigation = ModifiedDamage * SpellSuppressionMagnitude / 100.0f;
		ModifiedDamage -= SpellSuppressionMitigation;

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise,
			FString::Printf(TEXT("Suppressing spell damage. Damage suppressed: %f. New damage: %f."), SpellSuppressionMitigation, ModifiedDamage));
#endif
	}
	// ~ End of Suppression Spell Damage Mitigation

	const FGameplayModifierEvaluatedData& ModifierEvaluatedData = FGameplayModifierEvaluatedData(UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, ModifiedDamage);
	OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
	
#endif // WITH_SERVER_CODE
}

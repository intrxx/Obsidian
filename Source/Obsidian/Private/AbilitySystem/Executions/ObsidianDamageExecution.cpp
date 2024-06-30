// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Executions/ObsidianDamageExecution.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Obsidian/ObsidianGameplayTags.h"

struct FObsidianDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition ArmorDef;
	FGameplayEffectAttributeCaptureDefinition SpellSuppressionChanceDef;
	FGameplayEffectAttributeCaptureDefinition SpellSuppressionMagnitudeDef;

	FObsidianDamageStatics()
	{
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
	
	float MitigatedDamage = FullDamage;
	
	float Armor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().ArmorDef, EvaluationParameters, Armor);
	Armor = FMath::Max<float>(Armor, 0.0f);

	//TODO Only mitigate raw physical damage
	const float RawPhysicalDamageMitigation = Armor / (Armor + 5 * MitigatedDamage);
	MitigatedDamage -= RawPhysicalDamageMitigation;
	
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise,
		FString::Printf(TEXT("Reducing raw physical damage. Damage reduced: %f. New damage: %f."), RawPhysicalDamageMitigation, MitigatedDamage));
#endif // WITH_EDITOR or UE_BUILD_DEVELOPMENT
	
	float SpellSuppressionChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().SpellSuppressionChanceDef, EvaluationParameters, SpellSuppressionChance);
	SpellSuppressionChance = FMath::Max<float>(SpellSuppressionChance, 0.0f);

	//TODO Only attempt to suppress spell damage
	if(SpellSuppressionChance > FMath::RandRange(1.0f, 100.0f))
	{
		float SpellSuppressionMagnitude = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().SpellSuppressionMagnitudeDef, EvaluationParameters, SpellSuppressionMagnitude);
		SpellSuppressionMagnitude = FMath::Max<float>(SpellSuppressionMagnitude, 0.0f);

		const float SpellSuppressionMitigation = MitigatedDamage * SpellSuppressionMagnitude / 100.0f;
		MitigatedDamage -= SpellSuppressionMitigation;

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise,
			FString::Printf(TEXT("Suppressing spell damage. Damage suppressed: %f. New damage: %f."), SpellSuppressionMitigation, MitigatedDamage));
#endif // WITH_EDITOR or UE_BUILD_DEVELOPMENT
	}

	const FGameplayModifierEvaluatedData& ModifierEvaluatedData = FGameplayModifierEvaluatedData(UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, MitigatedDamage);
	OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
	
#endif // WITH_SERVER_CODE
}

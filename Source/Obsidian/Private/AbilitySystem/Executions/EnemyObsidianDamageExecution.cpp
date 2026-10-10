// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/Executions/EnemyObsidianDamageExecution.h"

#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "Obsidian/ObsidianLogCategories.h"


struct FObsidianEnemyDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition HitBlockChanceDef;
	FGameplayEffectAttributeCaptureDefinition SpellBlockChanceDef;
	
	FObsidianEnemyDamageStatics()
	{
		HitBlockChanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetHitBlockChanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		SpellBlockChanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianHeroAttributeSet::GetSpellBlockChanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const FObsidianEnemyDamageStatics& ObsidianEnemyDamageStatics()
{
	static FObsidianEnemyDamageStatics EnemyDamageStatics;
	return EnemyDamageStatics;
}

UEnemyObsidianDamageExecution::UEnemyObsidianDamageExecution()
{
	RelevantAttributesToCapture.Add(ObsidianEnemyDamageStatics().HitBlockChanceDef);
	RelevantAttributesToCapture.Add(ObsidianEnemyDamageStatics().SpellBlockChanceDef);
}

void UEnemyObsidianDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& InExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = InExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
	FGameplayEffectContext* Context = ContextHandle.Get();
	
	const FAggregatorEvaluateParameters EvaluationParameters;

	if(true) //TODO(intrxx) Check if the ability is hit based
	{
		float HitBlockChance = 0.0f;
		InExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianEnemyDamageStatics().HitBlockChanceDef, EvaluationParameters, HitBlockChance);
		HitBlockChance = FMath::Max<float>(HitBlockChance, 0.0f);

		if(HitBlockChance >= FMath::RandRange(1.0f, 100.0f))
		{
			FObsidianGameplayEffectContext* ObsidianContext = static_cast<FObsidianGameplayEffectContext*>(Context);
			ObsidianContext->SetIsBlockedAttack(true);
			
			const FGameplayModifierEvaluatedData& ModifierEvaluatedData = FGameplayModifierEvaluatedData(UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, 0.0f);
			OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
		
			UE_LOG(ObLogDamage, Verbose, TEXT("The hit was blocked. Chance to block was: %f. New damage: 0"), HitBlockChance);
		
			return;
		}
	}

	if(true) //TODO(intrxx) Check if the ability is a spell
	{
		float SpellBlockChance = 0.0f;
		InExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianEnemyDamageStatics().SpellBlockChanceDef, EvaluationParameters, SpellBlockChance);
		SpellBlockChance = FMath::Max<float>(SpellBlockChance, 0.0f);

		if(SpellBlockChance >= FMath::RandRange(1.0f, 100.0f))
		{
			FObsidianGameplayEffectContext* ObsidianContext = static_cast<FObsidianGameplayEffectContext*>(Context);
			ObsidianContext->SetIsBlockedAttack(true);
			
			const FGameplayModifierEvaluatedData& ModifierEvaluatedData = FGameplayModifierEvaluatedData(UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, 0.0f);
			OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
		
			UE_LOG(ObLogDamage, Verbose, TEXT("The spell was blocked. Chance to block was: %f. New damage: 0"), SpellBlockChance);
		
			return;
		}
	}
	
	Super::Execute_Implementation(InExecutionParams, OutExecutionOutput);
	
#endif // WITH_SERVER_CODE
}

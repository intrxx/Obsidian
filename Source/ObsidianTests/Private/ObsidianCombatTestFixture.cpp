// Copyright 2026 out of sCope team - intrxx

#include "ObsidianCombatTestFixture.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"

#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "AbilitySystem/Executions/ObsidianDamageExecution.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "ObsidianCombatTestActors.h"


namespace ObsidianCombatTestFixture
{
	constexpr float GuaranteedHitAccuracy = 10000.0f;
	constexpr float UnreachableMaxValue = 1000000.0f;

	void SetDefaultAttributes(UObsidianCommonAttributeSet* InAttributeSet)
	{
		InAttributeSet->SetMaxHealth(FObsidianCombatTestEnvironment::DefaultHealth);
		InAttributeSet->SetHealth(FObsidianCombatTestEnvironment::DefaultHealth);
		InAttributeSet->SetAccuracy(GuaranteedHitAccuracy);
		InAttributeSet->SetMaxStaggerMeter(UnreachableMaxValue);
	}

	template<typename CombatantClass>
	CombatantClass* SpawnCombatant(FActorTestSpawner& InSpawner)
	{
		// Combatants are spawned in the same spot, they do not need to move.
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		return &InSpawner.SpawnActor<CombatantClass>(SpawnParameters);
	}
}

FObsidianCombatTestEnvironment::FObsidianCombatTestEnvironment()
{
	Hero = ObsidianCombatTestFixture::SpawnCombatant<AObsidianTestCombatant_Hero>(Spawner);
	Enemy = ObsidianCombatTestFixture::SpawnCombatant<AObsidianTestCombatant_Enemy>(Spawner);

	// The Damage Executions and the Attribute Sets read the results of the damage from the Obsidian Effect Context.
	const FGameplayEffectContextHandle ContextHandle = Hero->GetCombatantAbilitySystemComponent()->MakeEffectContext();
	checkf(ContextHandle.IsValid() && ContextHandle.Get()->GetScriptStruct() == FObsidianGameplayEffectContext::StaticStruct(),
		TEXT("Effect Contexts are not FObsidianGameplayEffectContext, is UObsidianAbilitySystemGlobals still set in DefaultGame.ini?"));

	ObsidianCombatTestFixture::SetDefaultAttributes(Hero->GetCommonAttributeSet());
	ObsidianCombatTestFixture::SetDefaultAttributes(Enemy->GetCommonAttributeSet());

	// Reaching the Max Experience levels up the Hero, which only the real Hero with its Player State can do.
	Hero->GetHeroAttributeSet()->SetMaxExperience(ObsidianCombatTestFixture::UnreachableMaxValue);
}

float FObsidianCombatTestEnvironment::EvaluateMagnitudeCalculation(const TSubclassOf<UGameplayModMagnitudeCalculation>& InCalculationClass,
	const AObsidianTestCombatant& InCombatant)
{
	UObsidianAbilitySystemComponent* AbilitySystemComponent = InCombatant.GetCombatantAbilitySystemComponent();

	FCustomCalculationBasedFloat CustomCalculation;
	CustomCalculation.CalculationClassMagnitude = InCalculationClass;

	// The effect is never applied, the modifier is only there for the Spec to set up the captures of the calculation.
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage());
	Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers.AddDefaulted_GetRef();
	ModifierInfo.Attribute = UObsidianCommonAttributeSet::GetIncomingHealthHealingAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalculation);

	// Calculations get the level from the Source Object, in game it is the Character that applies its default attributes.
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(&InCombatant);

	FGameplayEffectSpec Spec(Effect, ContextHandle, 1.0f);
	Spec.CaptureAttributeDataFromTarget(AbilitySystemComponent);

	float Magnitude = 0.0f;
	const bool bCalculated = ModifierInfo.ModifierMagnitude.AttemptCalculateMagnitude(Spec, Magnitude);
	checkf(bCalculated, TEXT("Could not calculate the magnitude of [%s]."), *GetNameSafe(InCalculationClass));
	return Magnitude;
}

FObsidianTestDamageResult FObsidianCombatTestEnvironment::ApplyDamage(const AObsidianTestCombatant& InSource,
	const AObsidianTestCombatant& InTarget, const TMap<FGameplayTag, float>& InDamageTypes,
	const TSubclassOf<UGameplayEffectExecutionCalculation>& InExecutionClass)
{
	UObsidianAbilitySystemComponent* SourceASC = InSource.GetCombatantAbilitySystemComponent();
	UObsidianAbilitySystemComponent* TargetASC = InTarget.GetCombatantAbilitySystemComponent();
	const UObsidianCommonAttributeSet* TargetAttributeSet = InTarget.GetCommonAttributeSet();

	FGameplayEffectExecutionDefinition ExecutionDefinition;
	ExecutionDefinition.CalculationClass = InExecutionClass ? InExecutionClass : TSubclassOf<UGameplayEffectExecutionCalculation>(UObsidianDamageExecution::StaticClass());

	UGameplayEffect* DamageEffect = NewObject<UGameplayEffect>(GetTransientPackage());
	DamageEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	DamageEffect->Executions.Add(ExecutionDefinition);

	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(&InSource);

	FGameplayEffectSpec Spec(DamageEffect, ContextHandle, 1.0f);
	for (const TPair<FGameplayTag, float>& DamageType : InDamageTypes)
	{
		Spec.SetSetByCallerMagnitude(DamageType.Key, DamageType.Value);
	}

	const float HealthPoolBefore = TargetAttributeSet->GetHealth() + TargetAttributeSet->GetEnergyShield();
	SourceASC->ApplyGameplayEffectSpecToTarget(Spec, TargetASC);
	const float HealthPoolAfter = TargetAttributeSet->GetHealth() + TargetAttributeSet->GetEnergyShield();

	FObsidianTestDamageResult Result;
	Result.DamageTaken = HealthPoolBefore - HealthPoolAfter;

	// The Spec that gets executed is a copy, but it shares the context with this one.
	const FObsidianGameplayEffectContext* ObsidianContext = FObsidianGameplayEffectContext::ExtractEffectContextFromHandle(ContextHandle);
	Result.bIsBlockedAttack = ObsidianContext->IsBlockedAttack();
	Result.bIsCriticalAttack = ObsidianContext->IsCriticalAttack();
	Result.bIsEvadedHit = ObsidianContext->IsEvadedHit();
	Result.bIsSuppressedSpell = ObsidianContext->IsSuppressedSpell();
	Result.bIsTargetImmune = ObsidianContext->IsTargetImmune();
	return Result;
}

void FObsidianCombatTestEnvironment::ApplyInstantModifier(const AObsidianTestCombatant& InSource, const AObsidianTestCombatant& InTarget,
	const FGameplayAttribute& InAttribute, const float InMagnitude)
{
	UObsidianAbilitySystemComponent* SourceASC = InSource.GetCombatantAbilitySystemComponent();

	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage());
	Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers.AddDefaulted_GetRef();
	ModifierInfo.Attribute = InAttribute;
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FScalableFloat(InMagnitude);

	const FGameplayEffectSpec Spec(Effect, SourceASC->MakeEffectContext(), 1.0f);
	SourceASC->ApplyGameplayEffectSpecToTarget(Spec, InTarget.GetCombatantAbilitySystemComponent());
}

void FObsidianCombatTestEnvironment::AddTag(const AObsidianTestCombatant& InCombatant, const FGameplayTag& InTag)
{
	InCombatant.GetCombatantAbilitySystemComponent()->AddLooseGameplayTag(InTag);
}

#endif // WITH_DEV_AUTOMATION_TESTS

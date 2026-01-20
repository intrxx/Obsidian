// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/Executions/ObsidianDamageExecution.h"

// ~ Core
#include "AbilitySystemComponent.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Obsidian/ObsidianGameplayTags.h"

struct FObsidianDamageStatics
{
	// Source
	FGameplayEffectAttributeCaptureDefinition AccuracyDef;
	FGameplayEffectAttributeCaptureDefinition CriticalStrikeChanceDef;
	FGameplayEffectAttributeCaptureDefinition CriticalStrikeDamageMultiplierDef;
	FGameplayEffectAttributeCaptureDefinition ChanceToShockDef;
	FGameplayEffectAttributeCaptureDefinition IncreasedEffectOfShockDef;
	
	FGameplayEffectAttributeCaptureDefinition MinFlatPhysicalDamageDef;
	FGameplayEffectAttributeCaptureDefinition MaxFlatPhysicalDamageDef;
	FGameplayEffectAttributeCaptureDefinition MinFlatFireDamageDef;
	FGameplayEffectAttributeCaptureDefinition MaxFlatFireDamageDef;
	FGameplayEffectAttributeCaptureDefinition MinFlatLightningDamageDef;
	FGameplayEffectAttributeCaptureDefinition MaxFlatLightningDamageDef;
	FGameplayEffectAttributeCaptureDefinition MinFlatColdDamageDef;
	FGameplayEffectAttributeCaptureDefinition MaxFlatColdDamageDef;
	FGameplayEffectAttributeCaptureDefinition MinFlatChaosDamageDef;
	FGameplayEffectAttributeCaptureDefinition MaxFlatChaosDamageDef;
	
	// Target
	FGameplayEffectAttributeCaptureDefinition EvasionDef;
	FGameplayEffectAttributeCaptureDefinition ArmorDef;
	FGameplayEffectAttributeCaptureDefinition SpellSuppressionChanceDef;
	FGameplayEffectAttributeCaptureDefinition SpellSuppressionMagnitudeDef;
	FGameplayEffectAttributeCaptureDefinition AilmentThresholdDef;
	FGameplayEffectAttributeCaptureDefinition StaggerDamageTakenMultiplierDef;
	FGameplayEffectAttributeCaptureDefinition AllDamageMultiplierDef;
	
	FGameplayEffectAttributeCaptureDefinition FireResistanceDef;
	FGameplayEffectAttributeCaptureDefinition ColdResistanceDef;
	FGameplayEffectAttributeCaptureDefinition LightningResistanceDef;
	FGameplayEffectAttributeCaptureDefinition ChaosResistanceDef;
	
	FGameplayTagContainer UIDataTags;
	
	FObsidianDamageStatics()
	{
		// Source
		AccuracyDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetAccuracyAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		CriticalStrikeChanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetCriticalStrikeChanceAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		CriticalStrikeDamageMultiplierDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetCriticalStrikeDamageMultiplierAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		ChanceToShockDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetChanceToShockAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		IncreasedEffectOfShockDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetIncreasedEffectOfShockAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		
		MinFlatPhysicalDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMinFlatPhysicalDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		MaxFlatPhysicalDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMaxFlatPhysicalDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		MinFlatFireDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMinFlatFireDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		MaxFlatFireDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMaxFlatFireDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		MinFlatLightningDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMinFlatLightningDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		MaxFlatLightningDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMaxFlatLightningDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		MinFlatColdDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMinFlatColdDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		MaxFlatColdDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMaxFlatColdDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		MinFlatChaosDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMinFlatChaosDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		MaxFlatChaosDamageDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetMaxFlatChaosDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		
		// Target
		EvasionDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetEvasionAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		ArmorDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetArmorAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		SpellSuppressionChanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetSpellSuppressionChanceAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		SpellSuppressionMagnitudeDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetSpellSuppressionMagnitudeAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		AilmentThresholdDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetAilmentThresholdAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		StaggerDamageTakenMultiplierDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetStaggerDamageTakenMultiplierAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		AllDamageMultiplierDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetAllDamageMultiplierAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		
		FireResistanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetFireResistanceAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		ColdResistanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetColdResistanceAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		LightningResistanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetLightningResistanceAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		ChaosResistanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetChaosResistanceAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
		
		UIDataTags.AddTag(ObsidianGameplayTags::UI_DataSpecifierTag);
		UIDataTags.AddTag(ObsidianGameplayTags::UI_EffectData_Effect_Shock);
	}
};

static const FObsidianDamageStatics& ObsidianDamageStatics()
{
	static FObsidianDamageStatics ObsidianDamageStatics;
	return ObsidianDamageStatics;
}

UObsidianDamageExecution::UObsidianDamageExecution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Source
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().AccuracyDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().CriticalStrikeChanceDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().CriticalStrikeDamageMultiplierDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().ChanceToShockDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().IncreasedEffectOfShockDef);
	
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MinFlatPhysicalDamageDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MaxFlatPhysicalDamageDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MinFlatFireDamageDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MaxFlatFireDamageDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MinFlatLightningDamageDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MaxFlatLightningDamageDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MinFlatColdDamageDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MaxFlatColdDamageDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MinFlatChaosDamageDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().MaxFlatChaosDamageDef);
	
	// Target
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().EvasionDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().SpellSuppressionChanceDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().SpellSuppressionMagnitudeDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().AilmentThresholdDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().StaggerDamageTakenMultiplierDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().AllDamageMultiplierDef);

	RelevantAttributesToCapture.Add(ObsidianDamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().ColdResistanceDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().ChaosResistanceDef);
}

void UObsidianDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FObsidianGameplayEffectContext* ObsidianEffectContext = FObsidianGameplayEffectContext::ExtractEffectContextFromHandle(Spec.GetContext());
	checkf(ObsidianEffectContext, TEXT("Obsidian Gameplay Effect Context is invalid in Obsidian Damage Execution"));
	
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//~ Implementation of Immunity
	if(TargetTags->HasTag(ObsidianGameplayTags::Status_Immunity))
	{
		ObsidianEffectContext->SetIsTargetImmune(true);
		
		const FGameplayModifierEvaluatedData& DamageModifierEvaluatedData = FGameplayModifierEvaluatedData(
			UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, 0.0f);
		OutExecutionOutput.AddOutputModifier(DamageModifierEvaluatedData);
		
		return;
	}
	//~ End of Immunity implementation
	
	//TODO(intrxx) Only evade Hits
	// ~ Start of Hit Evasion Calculation
	float Evasion = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().EvasionDef, EvaluationParameters, Evasion);
	Evasion = FMath::Max<float>(Evasion, 0.0f);
	
	float Accuracy = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().AccuracyDef, EvaluationParameters, Accuracy);
	Accuracy = FMath::Max<float>(Accuracy, 0.0f);

	//TODO(intrxx) Read about entropy hit here: https://pathofexile.fandom.com/wiki/Evasion#cite_note-cite4-2,
	// https://www.pathofexile.com/forum/view-thread/11707#p216024 and maybe implement it
	
	float ChanceToHit = ((Accuracy * 1.25f) / ((Accuracy + FMath::Pow((Evasion * 0.25f), 0.8f)))) * 100.0f;
	ChanceToHit = FMath::Clamp<float>(ChanceToHit, 5.0f, 100.0f);
	
	if(!(ChanceToHit >= FMath::RandRange(1.0f, 100.0f))) // We did not hit return 0 damage
	{
		ObsidianEffectContext->SetIsEvadedHit(true);	
		
		const FGameplayModifierEvaluatedData& ModifierEvaluatedData = FGameplayModifierEvaluatedData(
			UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, 0.0f);
		OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
		
#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("The hit was evaded. Chance to hit was: [%f]. New damage: [0]"), ChanceToHit));
#endif
		
		return;
	}
	// ~ End of Evasion Calculation Hit
	
	// ~ Start of Capturing Damage
	float FullDamage = 0.0f;

	// ~~ Start of Physical Damage
	float PhysicalDamage = Spec.GetSetByCallerMagnitude(ObsidianGameplayTags::SetByCaller_DamageType_Physical, false, 0.0f);
	float MinPhysicalDamage = 0.0f;
	float MaxPhysicalDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MinFlatPhysicalDamageDef, EvaluationParameters, MinPhysicalDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MaxFlatPhysicalDamageDef, EvaluationParameters, MaxPhysicalDamage);
	PhysicalDamage += FMath::Max<float>(FMath::RandRange(static_cast<int32>(MinPhysicalDamage), static_cast<int32>(MaxPhysicalDamage)), 0.0f);
	
	if(PhysicalDamage > 0.0f)
	{
		// ~~~ Start of Armor Raw Physical Damage Mitigation
		float Armor = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().ArmorDef, EvaluationParameters, Armor);
		Armor = FMath::Max<float>(Armor, 0.0f);
	
		const float RawPhysicalDamageMitigation = Armor / (Armor + 5 * PhysicalDamage);
		PhysicalDamage -= RawPhysicalDamageMitigation;
	
#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			FString::Printf(TEXT("Reducing raw physical damage. Damage reduced: [%f]. New Physical damage: [%f]."),
				RawPhysicalDamageMitigation, PhysicalDamage));
#endif
		// ~~~ End of Armor Raw Physical Damage Mitigation

		FullDamage += PhysicalDamage;
	}
	// ~~ End of Physical Damage

	// ~~ Start of Fire Damage
	float FireDamage = Spec.GetSetByCallerMagnitude(ObsidianGameplayTags::SetByCaller_DamageType_Elemental_Fire, false, 0.0f);
	float MinFireDamage = 0.0f;
	float MaxFireDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MinFlatFireDamageDef, EvaluationParameters, MinFireDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MaxFlatFireDamageDef, EvaluationParameters, MaxFireDamage);
	FireDamage += FMath::Max<float>(FMath::RandRange(static_cast<int32>(MinFireDamage), static_cast<int32>(MaxFireDamage)), 0.0f);
	
	if (FireDamage > 0.0f)
	{
		// ~~~ Start of Fire Resistance Damage Mitigation
		float FireResistance = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().FireResistanceDef, EvaluationParameters, FireResistance);
		FireDamage *= (100.0f - FireResistance) / 100.0f;

#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			FString::Printf(TEXT("Reducing fire damage by resistance [%f]. New Fire damage: [%f]."),
				FireResistance, FireDamage));
#endif
		// ~~~ End of Fire Resistance Damage Mitigation

		FullDamage += FireDamage;
	}
	// ~~ End of Fire Damage

	// ~~ Start of Lightning Damage
	float LightningDamage = Spec.GetSetByCallerMagnitude(ObsidianGameplayTags::SetByCaller_DamageType_Elemental_Lightning, false, 0.0f);
	float MinLightningDamage = 0.0f;
	float MaxLightningDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MinFlatLightningDamageDef, EvaluationParameters, MinLightningDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MaxFlatLightningDamageDef, EvaluationParameters, MaxLightningDamage);
	LightningDamage += FMath::Max<float>(FMath::RandRange(static_cast<int32>(MinLightningDamage), static_cast<int32>(MaxLightningDamage)), 0.0f);

	if (LightningDamage > 0.0f)
	{
		// ~~~ Start of Lightning Resistance Damage Mitigation
		float LightningResistance = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().LightningResistanceDef, EvaluationParameters, LightningResistance);
		LightningDamage *= (100.0f - LightningResistance) / 100.0f;

#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			FString::Printf(TEXT("Reducing lightning damage by resistance [%f]. New Lightning damage: [%f]."),
				LightningResistance, LightningDamage));
#endif
		// ~~~ End of Lightning Resistance Damage Mitigation

		FullDamage += LightningDamage;
	}
	// ~~ End of Lightning Damage

	// ~~ Start of Cold Damage
	float ColdDamage = Spec.GetSetByCallerMagnitude(ObsidianGameplayTags::SetByCaller_DamageType_Elemental_Cold, false, 0.0f);
	float MinColdDamage = 0.0f;
	float MaxColdDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MinFlatColdDamageDef, EvaluationParameters, MinColdDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MaxFlatColdDamageDef, EvaluationParameters, MaxColdDamage);
	ColdDamage += FMath::Max<float>(FMath::RandRange(static_cast<int32>(MinColdDamage), static_cast<int32>(MaxColdDamage)), 0.0f);

	if (ColdDamage > 0.0f)
	{
		// ~~~ Start of Cold Resistance Damage Mitigation
		float ColdResistance = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().ColdResistanceDef, EvaluationParameters, ColdResistance);
		ColdDamage *= (100.0f - ColdResistance) / 100.0f;

#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			FString::Printf(TEXT("Reducing cold damage by resistance [%f]. New Cold damage: [%f]."),
				ColdResistance, ColdDamage));
#endif
		// ~~~ End of Cold Resistance Damage Mitigation

		FullDamage += ColdDamage;
	}
	// ~~ End of Cold Damage

	// ~~ Start of Chaos Damage
	float ChaosDamage = Spec.GetSetByCallerMagnitude(ObsidianGameplayTags::SetByCaller_DamageType_Chaos, false, 0.0f);
	float MinChaosDamage = 0.0f;
	float MaxChaosDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MinFlatChaosDamageDef, EvaluationParameters, MinChaosDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().MaxFlatChaosDamageDef, EvaluationParameters, MaxChaosDamage);
	ChaosDamage += FMath::Max<float>(FMath::RandRange(static_cast<int32>(MinChaosDamage), static_cast<int32>(MaxChaosDamage)), 0.0f);

	if (ChaosDamage > 0.0f)
	{
		// ~~~ Start of Chaos Resistance Damage Mitigation
		float ChaosResistance = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().ColdResistanceDef, EvaluationParameters, ChaosResistance);
		ChaosDamage *= (100.0f - ChaosResistance) / 100.0f;

#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			FString::Printf(TEXT("Reducing chaos damage by resistance [%f]. New Chaos damage: [%f]."),
				ChaosResistance, ChaosDamage));
#endif
		// ~~~ End of Chaos Resistance Damage Mitigation

		FullDamage += ChaosDamage;
	}
	// ~~ End of Chaos Damage

	//TODO(intrxx) Only attempt to suppress spell damage - in other words see if the GA is a spell and not a Hit
	// ~ Start of Suppression Spell Damage Calculation
	float SpellSuppressionChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().SpellSuppressionChanceDef, EvaluationParameters, SpellSuppressionChance);
	SpellSuppressionChance = FMath::Max<float>(SpellSuppressionChance, 0.0f);
	
	if(SpellSuppressionChance >= FMath::RandRange(1.0f, 100.0f))
	{
		ObsidianEffectContext->SetIsSuppressedSpell(true);
		
		float SpellSuppressionMagnitude = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().SpellSuppressionMagnitudeDef, EvaluationParameters, SpellSuppressionMagnitude);
		SpellSuppressionMagnitude = FMath::Max<float>(SpellSuppressionMagnitude, 0.0f);

		FullDamage -= FullDamage * SpellSuppressionMagnitude / 100.0f;

#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise,
			FString::Printf(TEXT("Suppressing spell damage. Percent Damage suppressed: [%f%%]. New damage: [%f]."),
				SpellSuppressionMagnitude, FullDamage));
#endif
	}
	// ~ End of Suppression Spell Damage Calculation
	
	float ModifiedDamage = FullDamage;

	//TODO(intrxx) Critical Strike Calculation might want to be on the ability itself
	// ~ Start of Critical Strikes Calculation
	float CriticalStrikeChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().CriticalStrikeChanceDef, EvaluationParameters, CriticalStrikeChance);
	CriticalStrikeChance = FMath::Max<float>(CriticalStrikeChance, 0.0f);
	
	if(CriticalStrikeChance >= FMath::RandRange(1.0f, 100.0f))
	{
		ObsidianEffectContext->SetIsCriticalAttack(true);	
		
		float CriticalStrikeDamageMultiplier = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().CriticalStrikeDamageMultiplierDef, EvaluationParameters, CriticalStrikeDamageMultiplier);
		CriticalStrikeDamageMultiplier = FMath::Max<float>(CriticalStrikeDamageMultiplier, 0.0f);
		
		ModifiedDamage = ModifiedDamage * (CriticalStrikeDamageMultiplier / 100.0f);
#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
			FString::Printf(TEXT("Critical Strike! New damage: [%f]."), ModifiedDamage));
#endif
	}
	// ~ End of Critical Strikes Calculation
	
	// ~ Start of Shock calculation
	float ChanceToShock = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().ChanceToShockDef, EvaluationParameters, ChanceToShock);
	ChanceToShock = FMath::Max<float>(ChanceToShock, 0.0f);
					
	if(ChanceToShock >= FMath::RandRange(1.0f, 100.0f))
	{
		float EnemyAilmentThreshold = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().AilmentThresholdDef, EvaluationParameters, EnemyAilmentThreshold);
		EnemyAilmentThreshold = FMath::Max<float>(EnemyAilmentThreshold, 1.0f);

		float IncreasedEffectOfShock = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().IncreasedEffectOfShockDef, EvaluationParameters, IncreasedEffectOfShock);
		IncreasedEffectOfShock = FMath::Max<float>(IncreasedEffectOfShock, 0.0f);

		float ShockEffect = (0.5f * FMath::Pow((LightningDamage / EnemyAilmentThreshold), 0.4f) * (1 + IncreasedEffectOfShock)) * 100.0f;
						
		if(ShockEffect > 5.0f)
		{
			ShockEffect = FMath::Min(ShockEffect, 50.0f);

			// EnemyAilmentThreshold is almost treated like Max Health here
			float ShockDuration = ((LightningDamage / (EnemyAilmentThreshold / 100.0f)) * 300.0f) / 1000.0f;
			ShockDuration = FMath::Clamp(ShockDuration, 0.0f, 12.0f);

			UGameplayEffect* GEShock = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Shock")));
			GEShock->DurationPolicy = EGameplayEffectDurationType::HasDuration;
			GEShock->DurationMagnitude = FGameplayEffectModifierMagnitude(ShockDuration);
			GEShock->Modifiers.SetNum(1);

			FGameplayModifierInfo& InfoShockDamageMultiplier = GEShock->Modifiers[0];
			InfoShockDamageMultiplier.Attribute = UObsidianCommonAttributeSet::GetShockDamageTakenMultiplierAttribute();
			InfoShockDamageMultiplier.ModifierMagnitude = FGameplayEffectModifierMagnitude(1 + (ShockEffect / 100.0f));
			InfoShockDamageMultiplier.ModifierOp = EGameplayModOp::Override;

			FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = FGameplayEffectSpecHandle(new FGameplayEffectSpec(GEShock, Context, 1.0f));
							
			SpecHandle.Data.Get()->AppendDynamicAssetTags(ObsidianDamageStatics().UIDataTags);

			//TODO(intrxx) Idk if this is the right way to do it, need research
			//TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(), TargetASC->GetPredictionKeyForNewAction());
		}
	}
	// ~ End of Shock calculation
	
	// ~ Start of Stagger calculation
	float StaggerDamageTakenMultiplier = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().StaggerDamageTakenMultiplierDef, EvaluationParameters, StaggerDamageTakenMultiplier);
	StaggerDamageTakenMultiplier = FMath::Max<float>(StaggerDamageTakenMultiplier, 0.0f);
	
	ModifiedDamage *= (100.0f + StaggerDamageTakenMultiplier) / 100.0f;
	
	//TODO(intrxx) Take Into account the weapon that the Player used to perform the attack
	float StaggerMagnitude = ((ModifiedDamage / 100.f) * 2.40f) * 100.0f;
	
	const FGameplayModifierEvaluatedData& StaggerModifierEvaluatedData = FGameplayModifierEvaluatedData(UObsidianCommonAttributeSet::GetIncomingStaggerMagnitudeAttribute(), EGameplayModOp::Override, StaggerMagnitude);
	OutExecutionOutput.AddOutputModifier(StaggerModifierEvaluatedData);
	// ~ End of Stagger calculation

	// ~ Start of All Damage Multiplier calculation
	float AllDamageMultiplier = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().AllDamageMultiplierDef, EvaluationParameters, AllDamageMultiplier);
	AllDamageMultiplier = FMath::Max<float>(AllDamageMultiplier, 0.0f);

	ModifiedDamage = ModifiedDamage * AllDamageMultiplier;
	// ~ End of All Damage Multiplier calculation
	
	const FGameplayModifierEvaluatedData& DamageModifierEvaluatedData = FGameplayModifierEvaluatedData(UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, ModifiedDamage);
	OutExecutionOutput.AddOutputModifier(DamageModifierEvaluatedData);
	
#endif // WITH_SERVER_CODE
}

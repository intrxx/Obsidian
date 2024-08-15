// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Executions/ObsidianDamageExecution.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Obsidian/Obsidian.h"
#include "Obsidian/ObsidianGameplayTags.h"

struct FObsidianDamageStatics
{
	// Source
	FGameplayEffectAttributeCaptureDefinition AccuracyDef;
	FGameplayEffectAttributeCaptureDefinition CriticalStrikeChanceDef;
	FGameplayEffectAttributeCaptureDefinition CriticalStrikeDamageMultiplierDef;
	FGameplayEffectAttributeCaptureDefinition ChanceToShockDef;
	FGameplayEffectAttributeCaptureDefinition IncreasedEffectOfShockDef;
	
	// Target
	FGameplayEffectAttributeCaptureDefinition EvasionDef;
	FGameplayEffectAttributeCaptureDefinition ArmorDef;
	FGameplayEffectAttributeCaptureDefinition SpellSuppressionChanceDef;
	FGameplayEffectAttributeCaptureDefinition SpellSuppressionMagnitudeDef;
	FGameplayEffectAttributeCaptureDefinition AilmentThresholdDef;
	
	FGameplayEffectAttributeCaptureDefinition FireResistanceDef;
	FGameplayEffectAttributeCaptureDefinition ColdResistanceDef;
	FGameplayEffectAttributeCaptureDefinition LightningResistanceDef;
	FGameplayEffectAttributeCaptureDefinition ChaosResistanceDef;

	FGameplayTagContainer UIDataTags;

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> DamageTypesToResistancesDefMap;

	FObsidianDamageStatics()
	{
		// Source
		AccuracyDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetAccuracyAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		CriticalStrikeChanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetCriticalStrikeChanceAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		CriticalStrikeDamageMultiplierDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetCriticalStrikeDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		ChanceToShockDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetChanceToShockAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		IncreasedEffectOfShockDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetIncreasedEffectOfShockAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
		
		// Target
		EvasionDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetEvasionAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		ArmorDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetArmorAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		SpellSuppressionChanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetSpellSuppressionChanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		SpellSuppressionMagnitudeDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetSpellSuppressionMagnitudeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		AilmentThresholdDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetAilmentThresholdAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		
		FireResistanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetFireResistanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		ColdResistanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetColdResistanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		LightningResistanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetLightningResistanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		ChaosResistanceDef = FGameplayEffectAttributeCaptureDefinition(UObsidianCommonAttributeSet::GetChaosResistanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);

		DamageTypesToResistancesDefMap.Add(ObsidianGameplayTags::SetByCaller_DamageType_Elemental_Fire, FireResistanceDef);
		DamageTypesToResistancesDefMap.Add(ObsidianGameplayTags::SetByCaller_DamageType_Elemental_Cold, ColdResistanceDef);
		DamageTypesToResistancesDefMap.Add(ObsidianGameplayTags::SetByCaller_DamageType_Elemental_Lightning, LightningResistanceDef);
		DamageTypesToResistancesDefMap.Add(ObsidianGameplayTags::SetByCaller_DamageType_Chaos, ChaosResistanceDef);
		
		UIDataTags.AddTag(ObsidianGameplayTags::Data_EffectUIInfo);
		UIDataTags.AddTag(ObsidianGameplayTags::UI_EffectData_Effect_Shock);
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
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().ChanceToShockDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().IncreasedEffectOfShockDef);
	
	// Target
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().EvasionDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().SpellSuppressionChanceDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().SpellSuppressionMagnitudeDef);
	RelevantAttributesToCapture.Add(ObsidianDamageStatics().AilmentThresholdDef);

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
		ObsidianEffectContext->SetIsEvadedHit(true);	
		
		const FGameplayModifierEvaluatedData& ModifierEvaluatedData = FGameplayModifierEvaluatedData(UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, 0.0f);
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

	//TODO This could be refactored to somehow get the actual DamageTypes of this ability that was used. 
	for(const FGameplayTag DamageType : ObsidianGameplayTags::DamageTypes)
	{
		if(DamageType == ObsidianGameplayTags::SetByCaller_DamageType_Physical)
		{
			float PhysicalDamage = Spec.GetSetByCallerMagnitude(DamageType, false, 0.0f);
			if(PhysicalDamage > 0.0f)
			{
				// ~ Start of Armor Raw Physical Damage Mitigation
				float Armor = 0.0f;
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().ArmorDef, EvaluationParameters, Armor);
				Armor = FMath::Max<float>(Armor, 0.0f);
	
				const float RawPhysicalDamageMitigation = Armor / (Armor + 5 * PhysicalDamage);
				PhysicalDamage -= RawPhysicalDamageMitigation;
				FullDamage += PhysicalDamage;
	
#if !UE_BUILD_SHIPPING
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
					FString::Printf(TEXT("Reducing raw physical damage. Damage reduced: [%f]. New damage: [%f]."), RawPhysicalDamageMitigation, FullDamage));
#endif
				// ~ End of Armor Raw Physical Damage Mitigation
			}
		}
		else if(DamageType == ObsidianGameplayTags::SetByCaller_DamageType_Elemental_Lightning)
		{
			float LightningDamage = Spec.GetSetByCallerMagnitude(DamageType, false, 0.0f);
			if(LightningDamage > 0.0f)
			{
				// ~ Start of Lightning Damage Mitigation
				float LightningResistance = 0.0f;
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().LightningResistanceDef, EvaluationParameters, LightningResistance);
				
				LightningDamage *= (100.0f - LightningResistance) / 100.0f;
				FullDamage += LightningDamage;
	
#if !UE_BUILD_SHIPPING
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
					FString::Printf(TEXT("Reducing damage by lightning resistance. Damage reduced: [%f]. New damage: [%f]."),
						LightningDamage, FullDamage));
#endif
				// ~ End of Lightning Damage Mitigation

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
						
						float ShockDuration = ((LightningDamage / ((EnemyAilmentThreshold * 2.0f) / 100.0f)) * 300.0f) / 1000.0f;
						ShockDuration = FMath::Clamp(ShockDuration, ShockDuration, ((EnemyAilmentThreshold * 2.0f) / 0.33f));

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
						
						TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(), TargetASC->GetPredictionKeyForNewAction());
					}
				}
				// ~ End of Shock calculation
			}
		}
		else // Capturing the rest of damage types and mitigate it by associated resistance - this will probably be captured one by one in the end
		{
			const FGameplayEffectAttributeCaptureDefinition ResistanceCaptureDef = ObsidianDamageStatics().DamageTypesToResistancesDefMap[DamageType];
			
			float ResistanceMitigatedDamage = Spec.GetSetByCallerMagnitude(DamageType, false, 0.0f);
			if(ResistanceMitigatedDamage > 0.0f)
			{
				float Resistance = 0.0f;
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResistanceCaptureDef, EvaluationParameters, Resistance);
				
				ResistanceMitigatedDamage *= (100.0f - Resistance) / 100.0f;

				FullDamage += ResistanceMitigatedDamage;

#if !UE_BUILD_SHIPPING
				if(Resistance > 0.0f)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
					FString::Printf(TEXT("Reducing damage by resistance [%s]. Damage reduced: [%f]. New damage: [%f]."),
						*ResistanceCaptureDef.AttributeToCapture.AttributeName, ResistanceMitigatedDamage, FullDamage));
				}
#endif
			}
		}
	}
	// ~ End of Capturing Damage
	
	float ModifiedDamage = FullDamage;

	//TODO Critical Strike Calculation might want to be on the ability itself
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
	
	//TODO Only attempt to suppress spell damage - in other words see if the GA is a spell and not a Hit
	// ~ Start of Suppression Spell Damage Mitigation
	float SpellSuppressionChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().SpellSuppressionChanceDef, EvaluationParameters, SpellSuppressionChance);
	SpellSuppressionChance = FMath::Max<float>(SpellSuppressionChance, 0.0f);
	
	if(SpellSuppressionChance >= FMath::RandRange(1.0f, 100.0f))
	{
		ObsidianEffectContext->SetIsSuppressedSpell(true);	
		
		float SpellSuppressionMagnitude = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ObsidianDamageStatics().SpellSuppressionMagnitudeDef, EvaluationParameters, SpellSuppressionMagnitude);
		SpellSuppressionMagnitude = FMath::Max<float>(SpellSuppressionMagnitude, 0.0f);

		const float SpellSuppressionMitigation = ModifiedDamage * SpellSuppressionMagnitude / 100.0f;
		ModifiedDamage -= SpellSuppressionMitigation;

#if !UE_BUILD_SHIPPING
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise,
			FString::Printf(TEXT("Suppressing spell damage. Damage suppressed: [%f]. New damage: [%f]."), SpellSuppressionMitigation, ModifiedDamage));
#endif
	}
	// ~ End of Suppression Spell Damage Mitigation

	const FGameplayModifierEvaluatedData& ModifierEvaluatedData = FGameplayModifierEvaluatedData(UObsidianCommonAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, ModifiedDamage);
	OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
	
#endif // WITH_SERVER_CODE
}

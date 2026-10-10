// Copyright 2026 out of sCope team - intrxx


#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "Characters/ObsidianCharacterBase.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "ObsidianTypes/ObsidianUITypes.h"


UObsidianCommonAttributeSet::UObsidianCommonAttributeSet()
	: StaggerMultiplier(1.0f)
	, AllDamageMultiplier(1.0f)
	, StaggerDamageTakenMultiplier(0.0f)
{
	bOutOfHealth = false;
}

void UObsidianCommonAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Vital
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, IncreasedHealthPercentage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, EnergyShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxEnergyShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, IncreasedEnergyShieldPercentage, COND_None, REPNOTIFY_Always);

	// Statuses
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, EnergyShieldRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, StaggerMeter, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxStaggerMeter, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, StaggerMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AllDamageMultiplier, COND_None, REPNOTIFY_Always);

	// Defence Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, IncreasedArmorPercent, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Evasion, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, IncreasedEvasionPercent, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, SpellSuppressionChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, SpellSuppressionMagnitude, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AilmentThreshold, COND_None, REPNOTIFY_Always);
	
	// Damage Taken
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ShockDamageTakenMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, StaggerDamageTakenMultiplier, COND_None, REPNOTIFY_Always);

	// Resistances
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxFireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ColdResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxColdResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxLightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ChaosResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxChaosResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AllElementalResistances, COND_None, REPNOTIFY_Always);

	// Status Effects
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, IncreasedEffectOfShock, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ChanceToShock, COND_None, REPNOTIFY_Always);

	// Damage Scaling Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Accuracy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CriticalStrikeChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CriticalStrikeDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CastSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, FirePenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ColdPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, LightningPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AllElementalPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ChaosPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, FireDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ColdDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, LightningDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AllElementalDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ChaosDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PhysicalDamageMultiplier, COND_None, REPNOTIFY_Always);
	
	// Base Damage Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MinFlatPhysicalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxFlatPhysicalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MinFlatFireDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxFlatFireDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MinFlatColdDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxFlatColdDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MinFlatLightningDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxFlatLightningDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MinFlatChaosDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxFlatChaosDamage, COND_None, REPNOTIFY_Always);
#if WITH_EDITOR // It fixes a crash with Abilities Gameplay Debugger, need more investigation why exactly
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseDamage, COND_None, REPNOTIFY_Always);
#else
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
#endif
	
	// Character
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MovementSpeed, COND_None, REPNOTIFY_Always);
}

void UObsidianCommonAttributeSet::PreAttributeChange(const FGameplayAttribute& InAttribute, float& InOutNewValue)
{
	Super::PreAttributeChange(InAttribute, InOutNewValue);

	if(InAttribute == GetHealthAttribute())
	{
		InOutNewValue = FMath::Clamp(InOutNewValue, 0.0f, GetMaxHealth());
	}
	else if(InAttribute == GetEnergyShieldAttribute())
	{
		InOutNewValue = FMath::Clamp(InOutNewValue, 0.0f, GetMaxEnergyShield());
	}

	if(bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UObsidianCommonAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& InData)
{
	Super::PostGameplayEffectExecute(InData);
	
	SetEffectProperties(InData, /** OUT */ EffectProps);
	
	if(InData.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if(InData.EvaluatedData.Attribute == GetEnergyShieldAttribute())
	{
		SetEnergyShield(FMath::Clamp(GetEnergyShield(), 0.0f, GetMaxEnergyShield()));
	}
	else if(InData.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		
		if(LocalIncomingDamage > 0.0f)
		{
			const float OldEnergyShield = GetEnergyShield();
			const float NewEnergyShield = OldEnergyShield - LocalIncomingDamage;
			SetEnergyShield(FMath::Clamp(NewEnergyShield, 0.0f, GetMaxEnergyShield()));
			
			if(NewEnergyShield < 0.0f)
			{
				// Damage that went through Energy Shield
				const float LocalRestOfDamage = NewEnergyShield;
				
				const float OldHealth = GetHealth();
				const float NewHealth = OldHealth + LocalRestOfDamage;
				SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

				if(!bOutOfHealth && (GetHealth() <= 0.0f))
				{
					if(OnOutOfHealth.IsBound())
					{
						OnOutOfHealth.Broadcast(EffectProps.Instigator, EffectProps.EffectCauser, &InData.EffectSpec,
							InData.EvaluatedData.Magnitude, OldHealth, NewHealth);
					}

					if(EffectProps.SourceCharacter->ActorHasTag(ObsidianActorTags::Player))
					{
						ApplyExperienceReward(EffectProps.SourceASC);
					}
				}
			}
		}

		//TODO(intrxx) Decide if I want to show damage numbers when Player gets damaged.
		// Show floating text - Logic is performed regardless of damage number because we might want to show blocked or evaded texts
		if(EffectProps.SourceCharacter != EffectProps.TargetCharacter && !EffectProps.bIsPlayerCharacter)
		{
			// If I decide that I want to show damage numbers for damaged Players - this cast will fail, so I will need to try the EffectProps.TargetController next.
			if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(EffectProps.SourceController))
			{
				FObsidianDamageTextProps DamageTextProps;
				DamageTextProps.DamageMagnitude = LocalIncomingDamage;
					
				const FGameplayEffectContextHandle EffectHandle = EffectProps.EffectContextHandle;
				if(const FObsidianGameplayEffectContext* ObsidianEffectContext = FObsidianGameplayEffectContext::ExtractEffectContextFromHandle(EffectHandle))
				{
					DamageTextProps.bIsBlockedAttack = ObsidianEffectContext->IsBlockedAttack();
					DamageTextProps.bIsCriticalAttack = ObsidianEffectContext->IsCriticalAttack();
					DamageTextProps.bIsEvadedHit = ObsidianEffectContext->IsEvadedHit();
					DamageTextProps.bIsSuppressedSpell = ObsidianEffectContext->IsSuppressedSpell();
					DamageTextProps.bIsTargetImmune = ObsidianEffectContext->IsTargetImmune();
				}
				ObsidianPC->ClientShowDamageNumber(DamageTextProps, EffectProps.TargetCharacter);
			}
		}
	}
	else if(InData.EvaluatedData.Attribute == GetIncomingStaggerMagnitudeAttribute())
	{
		const float LocalIncomingStaggerMagnitude = GetIncomingStaggerMagnitude();
		const float ModifiedIncomingStaggerMagnitude = FMath::FloorToInt(LocalIncomingStaggerMagnitude * GetStaggerMultiplier());
		const float CurrentStaggerMeter = GetStaggerMeter();
		const float NewStaggerMeter = CurrentStaggerMeter + ModifiedIncomingStaggerMagnitude;
		
		if(NewStaggerMeter > GetMaxStaggerMeter())
		{
			UAbilitySystemComponent* TargetASC = EffectProps.TargetASC;
			TargetASC->CancelAllAbilities();
			
			FGameplayTagContainer ActivateTag;
			ActivateTag.AddTag(ObsidianGameplayTags::AbilityActivation::Stagger);
			TargetASC->TryActivateAbilitiesByTag(ActivateTag);

			SetStaggerMeter(0.0f);
		}
		else
		{
			SetStaggerMeter(NewStaggerMeter);
		}

		SetIncomingStaggerMagnitude(0.0f);
	}
	else if(InData.EvaluatedData.Attribute == GetIncomingHealthHealingAttribute())
	{
		const float LocalIncomingHealthHealing = GetIncomingHealthHealing();
		
		if(LocalIncomingHealthHealing > 0.0f)
		{
			//TODO(intrxx) Handle Energy Shield based on some conditions in the future
			const float NewHealth = GetHealth() + LocalIncomingHealthHealing;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
		}
	}
	else if(InData.EvaluatedData.Attribute == GetIncomingEnergyShieldHealingAttribute())
	{
		const float LocalIncomingEnergyShieldHealing = GetIncomingEnergyShieldHealing();
		
		if(LocalIncomingEnergyShieldHealing > 0.0f)
		{
			const float NewEnergyShield = GetEnergyShield() + LocalIncomingEnergyShieldHealing;
			SetEnergyShield(FMath::Clamp(NewEnergyShield, 0.0f, GetMaxEnergyShield()));
		}
	}

	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UObsidianCommonAttributeSet::ResetMetaAttributes()
{
	SetIncomingDamage(0.0f);
	SetIncomingHealthHealing(0.0f);
	SetIncomingEnergyShieldHealing(0.0f);
}

void UObsidianCommonAttributeSet::ApplyExperienceReward(UAbilitySystemComponent* InSourceASC)
{
	// Create a dynamic instant Gameplay Effect to give the bounties
	UGameplayEffect* ExperienceRewardGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("ExperienceReward")));
	ExperienceRewardGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	ExperienceRewardGE->Modifiers.SetNum(1);

	float ExperienceToGive = /** Base TODO Maybe get it from monster later */ 200.0f /**TODO Get area level */ /**TODO Get monster Type */;
	
	FGameplayModifierInfo& ExperienceInfo = ExperienceRewardGE->Modifiers[0];
	ExperienceInfo.ModifierMagnitude = FScalableFloat(ExperienceToGive);
	ExperienceInfo.ModifierOp = EGameplayModOp::Additive;
	ExperienceInfo.Attribute = UObsidianHeroAttributeSet::GetExperienceAttribute();
	
	InSourceASC->ApplyGameplayEffectToSelf(ExperienceRewardGE, 1.0f, InSourceASC->MakeEffectContext());
}

void UObsidianCommonAttributeSet::OnRep_Health(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Health, InOldValue);

	const float CurrentHealth = GetHealth();
	const float Magnitude = CurrentHealth - InOldValue.GetCurrentValue();

	if(!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, Magnitude, InOldValue.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UObsidianCommonAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxHealth, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_IncreasedHealthPercentage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, IncreasedHealthPercentage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_EnergyShield(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, EnergyShield, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxEnergyShield(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxEnergyShield, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_IncreasedEnergyShieldPercentage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, IncreasedEnergyShieldPercentage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, HealthRegeneration, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_EnergyShieldRegeneration(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, EnergyShieldRegeneration, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_StaggerMeter(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, StaggerMeter, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxStaggerMeter(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxStaggerMeter, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_StaggerMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, StaggerMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_AllDamageMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AllDamageMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_Armor(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Armor, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_IncreasedArmorPercent(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, IncreasedArmorPercent, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_Evasion(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Evasion, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_IncreasedEvasionPercent(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, IncreasedEvasionPercent, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_SpellSuppressionChance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, SpellSuppressionChance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_SpellSuppressionMagnitude(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, SpellSuppressionMagnitude, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_AilmentThreshold(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AilmentThreshold, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_ShockDamageTakenMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ShockDamageTakenMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_StaggerDamageTakenMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, StaggerDamageTakenMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_AllElementalResistances(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AllElementalResistances, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, FireResistance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxFireResistance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxFireResistance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_ColdResistance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ColdResistance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxColdResistance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxColdResistance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, LightningResistance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxLightningResistance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxLightningResistance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_ChaosResistance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ChaosResistance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxChaosResistance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxChaosResistance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_IncreasedEffectOfShock(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, IncreasedEffectOfShock, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_ChanceToShock(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ChanceToShock, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_Accuracy(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Accuracy, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_CriticalStrikeChance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, CriticalStrikeChance, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_CriticalStrikeDamageMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, CriticalStrikeDamageMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AttackSpeed, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_CastSpeed(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, CastSpeed, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_FirePenetration(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, FirePenetration, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_ColdPenetration(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ColdPenetration, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_LightningPenetration(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, LightningPenetration, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_AllElementalPenetration(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AllElementalPenetration, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_ChaosPenetration(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ChaosPenetration, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_FireDamageMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, FireDamageMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_ColdDamageMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ColdDamageMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_LightningDamageMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, LightningDamageMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_AllElementalDamageMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AllElementalDamageMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_ChaosDamageMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ChaosDamageMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_PhysicalDamageMultiplier(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, PhysicalDamageMultiplier, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MinFlatPhysicalDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MinFlatPhysicalDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxFlatPhysicalDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxFlatPhysicalDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MinFlatFireDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MinFlatFireDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxFlatFireDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxFlatFireDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MinFlatColdDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MinFlatColdDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxFlatColdDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxFlatColdDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MinFlatLightningDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MinFlatLightningDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxFlatLightningDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxFlatLightningDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MinFlatChaosDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MinFlatChaosDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxFlatChaosDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxFlatChaosDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, BaseDamage, InOldValue);
}

void UObsidianCommonAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MovementSpeed, InOldValue);
}




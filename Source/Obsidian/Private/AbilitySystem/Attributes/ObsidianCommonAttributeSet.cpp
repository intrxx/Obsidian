// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Obsidian/ObsidianGameplayTags.h"
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
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, EnergyShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxEnergyShield, COND_None, REPNOTIFY_Always);

	// Statuses
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, EnergyShieldRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, StaggerMeter, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxStaggerMeter, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, StaggerMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AllDamageMultiplier, COND_None, REPNOTIFY_Always);

	// Defence Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Evasion, COND_None, REPNOTIFY_Always);
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

#if WITH_EDITOR // It fixes a crash with Abilities Gameplay Debugger, need more investigation why exactly
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseDamage, COND_None, REPNOTIFY_Always);
#else
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
#endif
	
	// Character
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MovementSpeed, COND_None, REPNOTIFY_Always);
}

void UObsidianCommonAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if(Attribute == GetEnergyShieldAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxEnergyShield());
	}

	if(bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UObsidianCommonAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	SetEffectProperties(Data, /** OUT */ EffectProps);
	
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if(Data.EvaluatedData.Attribute == GetEnergyShieldAttribute())
	{
		SetEnergyShield(FMath::Clamp(GetEnergyShield(), 0.0f, GetMaxEnergyShield()));
	}
	else if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
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
					OnOutOfHealth.Broadcast(EffectProps.Instigator, EffectProps.EffectCauser, &Data.EffectSpec, Data.EvaluatedData.Magnitude, OldHealth, NewHealth);
				}
			}
		}

		//TODO Decide if I want to show damage numbers when Player gets damaged.
		// Show floating text - Logic is performed regardless of damage number because we might want to show blocked or evaded hit
		if(EffectProps.SourceCharacter != EffectProps.TargetCharacter && !EffectProps.bIsPlayerCharacter)
		{
			// If I decide that I want to show damage numbers for damaged Players - this cast will fail, so I will need to cast the EffectProps.TargetController.
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
	else if(Data.EvaluatedData.Attribute == GetIncomingStaggerMagnitudeAttribute())
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
			ActivateTag.AddTag(ObsidianGameplayTags::AbilityActivation_Stagger);
			TargetASC->TryActivateAbilitiesByTag(ActivateTag);

			SetStaggerMeter(0.0f);
		}
		else
		{
			SetStaggerMeter(NewStaggerMeter);
		}

		SetIncomingStaggerMagnitude(0.0f);
	}
	else if(Data.EvaluatedData.Attribute == GetIncomingHealthHealingAttribute())
	{
		const float LocalIncomingHealthHealing = GetIncomingHealthHealing();
		
		if(LocalIncomingHealthHealing > 0.0f)
		{
			//TODO Handle Energy Shield based on some conditions in the future
			const float NewHealth = GetHealth() + LocalIncomingHealthHealing;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
		}
	}
	else if(Data.EvaluatedData.Attribute == GetIncomingEnergyShieldHealingAttribute())
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

void UObsidianCommonAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Health, OldValue);

	const float CurrentHealth = GetHealth();
	const float Magnitude = CurrentHealth - OldValue.GetCurrentValue();

	if(!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, Magnitude, OldValue.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UObsidianCommonAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxHealth, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_EnergyShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, EnergyShield, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxEnergyShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxEnergyShield, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, HealthRegeneration, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_EnergyShieldRegeneration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, EnergyShieldRegeneration, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_StaggerMeter(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, StaggerMeter, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxStaggerMeter(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxStaggerMeter, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_StaggerMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, StaggerMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_AllDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AllDamageMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Armor, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_Evasion(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Evasion, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_SpellSuppressionChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, SpellSuppressionChance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_SpellSuppressionMagnitude(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, SpellSuppressionMagnitude, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_AilmentThreshold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AilmentThreshold, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_ShockDamageTakenMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ShockDamageTakenMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_StaggerDamageTakenMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, StaggerDamageTakenMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_AllElementalResistances(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AllElementalResistances, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, FireResistance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxFireResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxFireResistance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_ColdResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ColdResistance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxColdResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxColdResistance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, LightningResistance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxLightningResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxLightningResistance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_ChaosResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ChaosResistance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxChaosResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxChaosResistance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_IncreasedEffectOfShock(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, IncreasedEffectOfShock, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_ChanceToShock(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ChanceToShock, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_Accuracy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Accuracy, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_CriticalStrikeChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, CriticalStrikeChance, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_CriticalStrikeDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, CriticalStrikeDamageMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AttackSpeed, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_CastSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, CastSpeed, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_FirePenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, FirePenetration, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_ColdPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ColdPenetration, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_LightningPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, LightningPenetration, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_AllElementalPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AllElementalPenetration, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_ChaosPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ChaosPenetration, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_FireDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, FireDamageMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_ColdDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ColdDamageMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_LightningDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, LightningDamageMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_AllElementalDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, AllElementalDamageMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_ChaosDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, ChaosDamageMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_PhysicalDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, PhysicalDamageMultiplier, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, BaseDamage, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MovementSpeed, OldValue);
}




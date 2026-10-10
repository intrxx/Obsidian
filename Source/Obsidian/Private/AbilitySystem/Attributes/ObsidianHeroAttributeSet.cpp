// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Heroes/ObsidianHero.h"


UObsidianHeroAttributeSet::UObsidianHeroAttributeSet()
	: MaxExperience(0.0f)
{
	bOutOfStamina = false;
}

void UObsidianHeroAttributeSet::PreAttributeChange(const FGameplayAttribute& InAttribute, float& InOutNewValue)
{
	Super::PreAttributeChange(InAttribute, InOutNewValue);

	if(InAttribute == GetManaAttribute())
	{
		InOutNewValue = FMath::Clamp(InOutNewValue, 0.0f, GetMaxMana());
	}
	else if (InAttribute == GetStaminaAttribute())
	{
		InOutNewValue = FMath::Clamp(InOutNewValue, 0.0f, GetMaxStamina());
	}
	
	if(bOutOfStamina && (GetStamina() > 0.0f))
	{
		bOutOfStamina = false;
	}
}

void UObsidianHeroAttributeSet::PostAttributeChange(const FGameplayAttribute& InAttribute, float InOldValue, float InNewValue)
{
	Super::PostAttributeChange(InAttribute, InOldValue, InNewValue);
	
	if(InAttribute == GetMaxHealthAttribute())
	{
		const float CurrentHealth = GetHealth();
		if(InNewValue < InOldValue) // If the New Max Attribute is lower, we need to always lower the base one
		{
			const float NewHealth = CurrentHealth - (InOldValue - InNewValue);
			SetHealth(FMath::Max<float>(NewHealth, 1.0f));		
		}
		else if(true) //TODO(intrxx) Check if hero in non-combat area (if equipping item)
		{
			const float NewHealth = CurrentHealth + (InNewValue - InOldValue);
			SetHealth(FMath::Max<float>(NewHealth, 1.0f));		
		}
	}
	else if(InAttribute == GetMaxEnergyShieldAttribute())
	{
		const float CurrentEnergyShield = GetEnergyShield();
		const float CurrentMaxEnergyShield = GetMaxEnergyShield();
		if(InNewValue < InOldValue) // If the New Max Attribute is lower, we need to always lower it
		{
			const float NewEnergyShield = CurrentEnergyShield - (InOldValue - InNewValue);
			SetEnergyShield(FMath::Clamp<float>(NewEnergyShield, 1.0f, CurrentMaxEnergyShield));		
		}
		else if(true) //TODO(intrxx) Check if hero in non-combat area (if equipping item)
		{
			const float NewEnergyShield = CurrentEnergyShield + (InNewValue - InOldValue);
			SetEnergyShield(FMath::Clamp<float>(NewEnergyShield, 1.0f, CurrentMaxEnergyShield));
		}
	}
	else if(InAttribute == GetMaxManaAttribute())
	{
		const float CurrentMana = GetMana();
		const float CurrentMaxMana = GetMaxMana();
		if(InNewValue < InOldValue) // If the New Max Attribute is lower, we need to always lower it
		{
			const float NewMana = CurrentMana - (InOldValue - InNewValue);
			SetMana(FMath::Clamp<float>(NewMana, 1.0f, CurrentMaxMana));		
		}
		else if(true) //TODO(intrxx) Check if hero in non-combat area (if equipping item)
		{
			const float NewMana = CurrentMana + (InNewValue - InOldValue);
			SetMana(FMath::Clamp<float>(NewMana, 1.0f, CurrentMaxMana));		
		}
	}
	else if(InAttribute == GetMaxSpecialResourceAttribute())
	{
		const float CurrentSpecialResource = GetSpecialResource();
		const float CurrentMaxSpecialResource = GetMaxSpecialResource();
		if(InNewValue < InOldValue) // If the New Max Attribute is lower, we need to always lower it
		{
			const float NewSpecialResource = CurrentSpecialResource - (InOldValue - InNewValue);
			SetSpecialResource(FMath::Clamp<float>(NewSpecialResource, 1.0f, CurrentMaxSpecialResource));		
		}
		else if(true) //TODO(intrxx) Check if hero in non-combat area (if equipping item)
		{
			const float NewSpecialResource = CurrentSpecialResource + (InNewValue - InOldValue);
			SetSpecialResource(FMath::Clamp<float>(NewSpecialResource, 1.0f, CurrentMaxSpecialResource));		
		}
	}
	else if(InAttribute == GetMaxStaminaAttribute())
	{
		const float CurrentStamina = GetStamina();
		const float CurrentMaxStamina = GetMaxStamina();
		if(InNewValue < InOldValue) // If the New Max Attribute is lower, we need to always lower it
		{
			const float NewStamina = CurrentStamina - (InOldValue - InNewValue);
			SetStamina(FMath::Clamp<float>(NewStamina, 1.0f, CurrentMaxStamina));		
		}
		else if(true) //TODO(intrxx) Check if hero in non-combat area (if equipping item)
		{
			const float NewStamina = CurrentStamina + (InNewValue - InOldValue);
			SetStamina(FMath::Clamp<float>(NewStamina, 1.0f, CurrentMaxStamina));		
		}
	}
}

void UObsidianHeroAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	/**
	 * Character
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PassiveSkillPoints, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AscensionPoints, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, SprintSpeed, COND_None, REPNOTIFY_Always);
	
	/**
	 * Spending attributes
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, IncreasedManaPercentage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, SpecialResource, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxSpecialResource, COND_None, REPNOTIFY_Always);

	/**
	 * Status
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, StaminaRegeneration, COND_None, REPNOTIFY_Always);

	/**
	 * "RPG Attributes"
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Faith, COND_None, REPNOTIFY_Always);

	/**
	 * Defence Attributes
	 */

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HitBlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHitBlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, SpellBlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxSpellBlockChance, COND_None, REPNOTIFY_Always);

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UObsidianHeroAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& InData)
{
	Super::PostGameplayEffectExecute(InData);

	//FObsidianEffectProperties EffectProps;
	//SetEffectProperties(Data, /** OUT */ EffectProps);

	if(InData.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
	else if(InData.EvaluatedData.Attribute == GetIncomingManaReplenishingAttribute())
	{
		const float LocalIncomingManaReplenishing = GetIncomingManaReplenishing();
		
		if(LocalIncomingManaReplenishing > 0.f)
		{
			const float NewMana = GetMana() + LocalIncomingManaReplenishing;
			SetMana(FMath::Clamp(NewMana, 0.f, GetMaxMana()));
		}
	}
	else if(InData.EvaluatedData.Attribute == GetExperienceAttribute())
	{
		const float CurrentExperience = GetExperience();
		const float CurrentMaxExperience = GetMaxExperience();

		if(CurrentExperience > CurrentMaxExperience)
		{
			const AObsidianHero* HeroCharacter = Cast<AObsidianHero>(EffectProps.SourceCharacter);
			checkf(HeroCharacter, TEXT("HeroCharacter is invalid while trying to level up character in UObsidianHeroAttributeSet::PostGameplayEffectExecute."));
			if(HeroCharacter)
			{
				HeroCharacter->IncreaseHeroLevel();
				
				const UObsidianHeroAttributesComponent* HeroAttributesComponent = UObsidianHeroAttributesComponent::FindHeroAttributesComponent(HeroCharacter);
				checkf(HeroAttributesComponent, TEXT("HeroCharacter has no HeroAttributesComponent in UObsidianHeroAttributeSet::PostGameplayEffectExecute."));
				if(HeroAttributesComponent)
				{
					const UGameplayEffect* LevelUpEffect = HeroAttributesComponent->GetLevelUpEffect();
					if(ensureMsgf(LevelUpEffect, TEXT("Level Up reward Effect is invalid for Hero Character.")))
					{
						if(UAbilitySystemComponent* SourceASC = EffectProps.SourceASC)
						{
							SourceASC->ApplyGameplayEffectToSelf(LevelUpEffect, (float)HeroCharacter->GetHeroLevel(), SourceASC->MakeEffectContext());
						}
					}
				}
			}
			
			const float NewMaxExperience = GetMaxExperienceForLevel(HeroCharacter->GetHeroLevel());
			SetMaxExperience(NewMaxExperience);
		}
	}
	else if (InData.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		const float NewStamina = GetStamina();
		const float OldStamina = FMath::Clamp(NewStamina + InData.EvaluatedData.Magnitude, 0.0f, GetMaxStamina());
		if(!bOutOfStamina && (GetStamina() <= 0.0f))
		{
			if(OnOutOfStamina.IsBound())
			{
				OnOutOfStamina.Broadcast(EffectProps.Instigator, EffectProps.EffectCauser, &InData.EffectSpec,
					InData.EvaluatedData.Magnitude, OldStamina, NewStamina);
			}
		}
	}

	bOutOfStamina = (GetStamina() <= 0.0f);

	ResetMetaAttributes();
	EffectProps.Reset();
}

float UObsidianHeroAttributeSet::GetMaxExperienceForLevel(const uint8 InHeroLevel)
{
	if(InHeroLevel <= 50)
	{
		return 125.0f * FMath::Pow(InHeroLevel, 1.4) + 350.0f * InHeroLevel;
	}
	return 250.0f * FMath::Pow(InHeroLevel, 1.8) + 500.0f * InHeroLevel;
}

void UObsidianHeroAttributeSet::ResetMetaAttributes()
{
	Super::ResetMetaAttributes();

	SetIncomingManaReplenishing(0.0f);
}

void UObsidianHeroAttributeSet::OnRep_Mana(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Mana, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxMana, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_IncreasedManaPercentage(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, IncreasedManaPercentage, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_SpecialResource(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, SpecialResource, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxSpecialResource(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxSpecialResource, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, ManaRegeneration, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_StaminaRegeneration(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, StaminaRegeneration, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_Experience(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Experience, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxExperience(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxExperience, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_PassiveSkillPoints(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, PassiveSkillPoints, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_AscensionPoints(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, AscensionPoints, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_Stamina(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Stamina, InOldValue);

	const float CurrentStamina = GetStamina();
	const float Magnitude = CurrentStamina - InOldValue.GetCurrentValue();

	if(!bOutOfStamina && CurrentStamina <= 0.0f)
	{
		OnOutOfStamina.Broadcast(nullptr, nullptr, nullptr, Magnitude, InOldValue.GetCurrentValue(), CurrentStamina);
	}

	bOutOfStamina = (CurrentStamina <= 0.0f);
}

void UObsidianHeroAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxStamina, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_SprintSpeed(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, SprintSpeed, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_HitBlockChance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, HitBlockChance, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxHitBlockChance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxHitBlockChance, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_SpellBlockChance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, SpellBlockChance, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxSpellBlockChance(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxSpellBlockChance, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_Strength(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Strength, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Intelligence, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_Dexterity(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Dexterity, InOldValue);
}

void UObsidianHeroAttributeSet::OnRep_Faith(const FGameplayAttributeData& InOldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Faith, InOldValue);
}
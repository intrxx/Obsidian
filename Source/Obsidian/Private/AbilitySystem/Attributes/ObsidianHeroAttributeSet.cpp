// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Net/UnrealNetwork.h"

UObsidianHeroAttributeSet::UObsidianHeroAttributeSet()
	: MaxExperience(0.0f)
{
}

void UObsidianHeroAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
}

void UObsidianHeroAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if(Attribute == GetMaxHealthAttribute())
	{
		const float CurrentHealth = GetHealth();
		if(NewValue < OldValue) // If the New Max Attribute is lower, we need to always lower the base one
		{
			const float NewHealth = CurrentHealth - (OldValue - NewValue);
			SetHealth(FMath::Max<float>(NewHealth, 1.0f));		
		}
		else if(true) //TODO Check if hero in non-combat area (if equipping item)
		{
			const float NewHealth = CurrentHealth + (NewValue - OldValue);
			SetHealth(FMath::Max<float>(NewHealth, 1.0f));		
		}
	}
	else if(Attribute == GetMaxEnergyShieldAttribute())
	{
		const float CurrentEnergyShield = GetEnergyShield();
		const float CurrentMaxEnergyShield = GetMaxEnergyShield();
		if(NewValue < OldValue) // If the New Max Attribute is lower, we need to always lower it
		{
			const float NewEnergyShield = CurrentEnergyShield - (OldValue - NewValue);
			SetEnergyShield(FMath::Clamp<float>(NewEnergyShield, 1.0f, CurrentMaxEnergyShield));		
		}
		else if(true) //TODO Check if hero in non-combat area (if equipping item)
		{
			const float NewEnergyShield = CurrentEnergyShield + (NewValue - OldValue);
			SetEnergyShield(FMath::Clamp<float>(NewEnergyShield, 1.0f, CurrentMaxEnergyShield));
		}
	}
	else if(Attribute == GetMaxManaAttribute())
	{
		const float CurrentMana = GetMana();
		const float CurrentMaxMana = GetMaxMana();
		if(NewValue < OldValue) // If the New Max Attribute is lower, we need to always lower it
		{
			const float NewMana = CurrentMana - (OldValue - NewValue);
			SetMana(FMath::Clamp<float>(NewMana, 1.0f, CurrentMaxMana));		
		}
		else if(true) //TODO Check if hero in non-combat area (if equipping item)
		{
			const float NewMana = CurrentMana + (NewValue - OldValue);
			SetMana(FMath::Clamp<float>(NewMana, 1.0f, CurrentMaxMana));		
		}
	}
	else if(Attribute == GetMaxSpecialResourceAttribute())
	{
		const float CurrentSpecialResource = GetSpecialResource();
		const float CurrentMaxSpecialResource = GetMaxSpecialResource();
		if(NewValue < OldValue) // If the New Max Attribute is lower, we need to always lower it
		{
			const float NewSpecialResource = CurrentSpecialResource - (OldValue - NewValue);
			SetSpecialResource(FMath::Clamp<float>(NewSpecialResource, 1.0f, CurrentMaxSpecialResource));		
		}
		else if(true) //TODO Check if hero in non-combat area (if equipping item)
		{
			const float NewSpecialResource = CurrentSpecialResource + (NewValue - OldValue);
			SetSpecialResource(FMath::Clamp<float>(NewSpecialResource, 1.0f, CurrentMaxSpecialResource));		
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
	
	/**
	 * Spending attributes
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, SpecialResource, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxSpecialResource, COND_None, REPNOTIFY_Always);

	/**
	 * Status
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ManaRegeneration, COND_None, REPNOTIFY_Always);

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

void UObsidianHeroAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//FObsidianEffectProperties EffectProps;
	//SetEffectProperties(Data, /** OUT */ EffectProps);

	if(Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
	else if(Data.EvaluatedData.Attribute == GetIncomingManaReplenishingAttribute())
	{
		const float LocalIncomingManaReplenishing = GetIncomingManaReplenishing();
		
		if(LocalIncomingManaReplenishing > 0.f)
		{
			const float NewMana = GetMana() + LocalIncomingManaReplenishing;
			SetMana(FMath::Clamp(NewMana, 0.f, GetMaxMana()));
		}
	}
	else if(Data.EvaluatedData.Attribute == GetExperienceAttribute())
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

				UObsidianHeroAttributesComponent* HeroAttributesComponent = UObsidianHeroAttributesComponent::FindAttributesComponent(HeroCharacter);
				checkf(HeroAttributesComponent, TEXT("HeroCharacter has no HeroAttributesComponent in UObsidianHeroAttributeSet::PostGameplayEffectExecute."));
				if(HeroAttributesComponent)
				{
					const UGameplayEffect* LevelUpEffect = HeroAttributesComponent->GetLevelUpEffect();
					if(ensureMsgf(LevelUpEffect, TEXT("Level Up reward Effect is invalid for Hero Character.")))
					{
						if(UAbilitySystemComponent* SourceASC = EffectProps.SourceASC)
						{
							SourceASC->ApplyGameplayEffectToSelf(LevelUpEffect, 1.0f, SourceASC->MakeEffectContext());
						}
					}
				}
			}
			
			float NewMaxExperience = 0.0f;
			const int32 CurrentHeroLevel = HeroCharacter->GetHeroLevel();
			if(CurrentHeroLevel <= 50)
			{
				NewMaxExperience = 125.0f * FMath::Pow(CurrentHeroLevel, 1.4) + 350.0f * CurrentHeroLevel;
			}
			else
			{
				NewMaxExperience = 250.0f * FMath::Pow(CurrentHeroLevel, 1.8) + 500.0f * CurrentHeroLevel;
			}

			SetMaxExperience(NewMaxExperience);
		}
	}

	ResetMetaAttributes();
	EffectProps.Reset();
}

void UObsidianHeroAttributeSet::ResetMetaAttributes()
{
	Super::ResetMetaAttributes();

	SetIncomingManaReplenishing(0.0f);
}

void UObsidianHeroAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Mana, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxMana, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_SpecialResource(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, SpecialResource, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxSpecialResource(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxSpecialResource, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, ManaRegeneration, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Experience, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxExperience, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_HitBlockChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, HitBlockChance, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxHitBlockChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxHitBlockChance, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_SpellBlockChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, SpellBlockChance, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_MaxSpellBlockChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, MaxSpellBlockChance, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Strength, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Intelligence, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_Dexterity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Dexterity, OldValue);
}

void UObsidianHeroAttributeSet::OnRep_Faith(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianHeroAttributeSet, Faith, OldValue);
}
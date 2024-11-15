// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UObsidianHeroAttributeSet::UObsidianHeroAttributeSet()
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

void UObsidianHeroAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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
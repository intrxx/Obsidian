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
	 * Spending attributes
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, SpecialResource, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, MaxSpecialResource, COND_None, REPNOTIFY_Always);

	/**
	 * Status
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	/**
	 * "RPG Attributes"
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, Faith, COND_None, REPNOTIFY_Always);

	/**
	 * Defence Attributes
	 */

	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, HitBlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, MaxHitBlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, SpellBlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianHeroAttributeSet, MaxSpellBlockChance, COND_None, REPNOTIFY_Always);
}

void UObsidianHeroAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FObsidianEffectProperties EffectProps;
	SetEffectProperties(Data, /** OUT */ EffectProps);

	if(Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
	else if(Data.EvaluatedData.Attribute == GetIncomingManaReplenishingAttribute())
	{
		const float LocalIncomingManaReplenishing = GetIncomingManaReplenishing();
		SetIncomingManaReplenishing(0.f);
		
		if(LocalIncomingManaReplenishing > 0.f)
		{
			const float NewMana = GetMana() + LocalIncomingManaReplenishing;
			SetMana(FMath::Clamp(NewMana, 0.f, GetMaxMana()));
		}
	}
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
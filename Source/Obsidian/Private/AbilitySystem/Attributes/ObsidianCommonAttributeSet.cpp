// Copyright 2024 Michał Ogiński


#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Net/UnrealNetwork.h"

UObsidianCommonAttributeSet::UObsidianCommonAttributeSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
{
}

void UObsidianCommonAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, Evasion, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, EnergyShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, MaxFireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, ColdResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, MaxColdResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, MaxLightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, ChaosResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianCommonAttributeSet, MaxChaosResistance, COND_None, REPNOTIFY_Always);
}

void UObsidianCommonAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Health, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, MaxHealth, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Armor, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_Evasion(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, Evasion, OldValue);
}

void UObsidianCommonAttributeSet::OnRep_EnergyShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianCommonAttributeSet, EnergyShield, OldValue);
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

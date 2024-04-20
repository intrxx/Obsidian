// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"

#include "Net/UnrealNetwork.h"

UObsidianEnemyAttributeSet::UObsidianEnemyAttributeSet()
	: EnemySpecificAttribute(10.0f)
{
}

void UObsidianEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UObsidianEnemyAttributeSet, EnemySpecificAttribute, COND_None, REPNOTIFY_Always);
}

void UObsidianEnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FObsidianEffectProperties EffectProps;
	SetEffectProperties(Data, /** OUT */ EffectProps);
}

void UObsidianEnemyAttributeSet::OnRep_EnemySpecificAttribute(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianEnemyAttributeSet, EnemySpecificAttribute, OldValue);
}

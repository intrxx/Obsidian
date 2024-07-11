// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/ObsidianAbilitySystemGlobals.h"

#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"

FGameplayEffectContext* UObsidianAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FObsidianGameplayEffectContext();
}

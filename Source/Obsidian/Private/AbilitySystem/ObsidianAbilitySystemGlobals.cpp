// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/ObsidianAbilitySystemGlobals.h"

// ~ Core

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"

FGameplayEffectContext* UObsidianAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FObsidianGameplayEffectContext();
}

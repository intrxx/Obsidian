// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/ObsidianDamageGameplayAbility.h"

UObsidianDamageGameplayAbility::UObsidianDamageGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

float FObsidianAbilityDamageRange::RollForDamageNumberAtLevel(const float Level) const
{
    const float MinValue = MinimalDamage.GetValueAtLevel(Level);
    const float MaxValue = MaximalDamage.GetValueAtLevel(Level);
    
    return FMath::FloorToFloat(FMath::RandRange(MinValue, MaxValue));
}



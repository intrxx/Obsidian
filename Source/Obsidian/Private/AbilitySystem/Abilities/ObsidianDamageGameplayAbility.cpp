// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/ObsidianDamageGameplayAbility.h"

#include "Obsidian/Obsidian.h"

UObsidianDamageGameplayAbility::UObsidianDamageGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UAnimMontage* UObsidianDamageGameplayAbility::GetAnimMontageToPlay()
{
    const uint16 ArrCount = AttackMontages.Num();
    if(ArrCount == 0)
    {
        UE_LOG(LogObsidian, Error, TEXT("Attack Montages are empty on [%s]."), *GetNameSafe(this));
        return nullptr;
    }

    // Handling this case exclusively as this is very probable case
    if(ArrCount == 1)
    {
        return AttackMontages[0];
    }

    const uint16 MontageNumber = FMath::RandRange(0, ArrCount - 1);
    return AttackMontages[MontageNumber];
}

float FObsidianAbilityDamageRange::RollForDamageNumberAtLevel(const float Level) const
{
    const float MinValue = MinimalDamage.GetValueAtLevel(Level);
    const float MaxValue = MaximalDamage.GetValueAtLevel(Level);
    
    return FMath::FloorToFloat(FMath::RandRange(MinValue, MaxValue));
}



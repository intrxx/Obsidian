// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/ObsidianDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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

void UObsidianDamageGameplayAbility::DamageAllCharacters(TArray<AActor*>& ActorsToDamage)
{
    if(ActorsToDamage.IsEmpty())
    {
        return;
    }
    
    const UAbilitySystemComponent* OwningASC = GetAbilitySystemComponentFromActorInfo();
	
    FGameplayEffectContextHandle ContextHandle = OwningASC->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	
    const FGameplayEffectSpecHandle SpecHandle = OwningASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);

    for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : DamageTypeMap)
    {
        const float Damage = Pair.Value.RollForDamageNumberAtLevel(GetAbilityLevel());
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Damage);
    }
    
    for(AActor* Actor : ActorsToDamage)
    {
        if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
        {
            TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
}

float FObsidianAbilityDamageRange::RollForDamageNumberAtLevel(const float Level) const
{
    const float MinValue = MinimalDamage.GetValueAtLevel(Level);
    const float MaxValue = MaximalDamage.GetValueAtLevel(Level);
    
    return FMath::FloorToFloat(FMath::RandRange(MinValue, MaxValue));
}



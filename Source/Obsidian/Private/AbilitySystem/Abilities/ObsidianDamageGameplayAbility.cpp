// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/ObsidianDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Obsidian/Obsidian.h"

UObsidianDamageGameplayAbility::UObsidianDamageGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UObsidianDamageGameplayAbility::DamageAllCharacters(TArray<AActor*>& ActorsToDamage)
{
    if(ActorsToDamage.IsEmpty())
    {
        return;
    }
    
    const UAbilitySystemComponent* OwningASC = GetAbilitySystemComponentFromActorInfo();
	const float AbilityLevel = GetAbilityLevel();
    
    FGameplayEffectContextHandle ContextHandle = OwningASC->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	
    const FGameplayEffectSpecHandle SpecHandle = OwningASC->MakeOutgoingSpec(DamageEffectClass, AbilityLevel, ContextHandle);

    for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : DamageTypeMap)
    {
        const float Damage = Pair.Value.RollForDamageNumberAtLevel(AbilityLevel);
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

void UObsidianDamageGameplayAbility::DamageCharacter(AActor* ActorToDamage)
{
    if(!IsValid(ActorToDamage))
    {
        return;
    }

    const UAbilitySystemComponent* OwningASC = GetAbilitySystemComponentFromActorInfo();
    const float AbilityLevel = GetAbilityLevel();
    
    FGameplayEffectContextHandle ContextHandle = OwningASC->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	
    const FGameplayEffectSpecHandle SpecHandle = OwningASC->MakeOutgoingSpec(DamageEffectClass, AbilityLevel, ContextHandle);

    for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : DamageTypeMap)
    {
        const float Damage = Pair.Value.RollForDamageNumberAtLevel(AbilityLevel);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Damage);
    }

    if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToDamage))
    {
        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

float FObsidianAbilityDamageRange::RollForDamageNumberAtLevel(const float Level) const
{
    const float MinValue = MinimalDamage.GetValueAtLevel(Level);
    const float MaxValue = MaximalDamage.GetValueAtLevel(Level);
    
    return FMath::FloorToFloat(FMath::RandRange(MinValue, MaxValue));
}



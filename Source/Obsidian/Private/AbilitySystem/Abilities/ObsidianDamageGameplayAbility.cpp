// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/ObsidianDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Combat/ObsidianCombatInterface.h"


UObsidianDamageGameplayAbility::UObsidianDamageGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UObsidianDamageGameplayAbility::DamageAllCharacters(const TArray<AActor*>& ActorsToDamage)
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

    for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : BaseDamageTypeMap)
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

    for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : BaseDamageTypeMap)
    {
        const float Damage = Pair.Value.RollForDamageNumberAtLevel(AbilityLevel);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Damage);
    }

    if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToDamage))
    {
        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

void UObsidianDamageGameplayAbility::SpawnBloodEffect(AActor* Actor, const FVector& BloodOriginLocation, UNiagaraSystem* OptionalBloodEffect)
{
    if(OptionalBloodEffect == nullptr)
    {
        UNiagaraSystem* BloodEffect = IObsidianCombatInterface::Execute_GetBloodEffect(Actor);
        if(BloodEffect == nullptr)
        {
            return;
        }

        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BloodEffect, BloodOriginLocation);
    }
    
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, OptionalBloodEffect, BloodOriginLocation);
}

void UObsidianDamageGameplayAbility::SpawnBloodEffects(const TArray<AActor*>& Actors, UNiagaraSystem* OptionalBloodEffect)
{
    if(Actors.IsEmpty())
    {
        return;
    }

    if(Actors.Num() == 1)
    {
        AActor* SingleActor = Actors[0];
        SpawnBloodEffect(SingleActor, SingleActor->GetActorLocation(), OptionalBloodEffect);
        return;
    }
    
    if(OptionalBloodEffect == nullptr)
    {
        for(AActor* Actor : Actors)
        {
            UNiagaraSystem* BloodEffect = IObsidianCombatInterface::Execute_GetBloodEffect(Actor);
            if(BloodEffect == nullptr)
            {
                continue;
            }
        
            const FVector BloodOriginLocation = Actor->GetActorLocation();
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BloodEffect, BloodOriginLocation);
        }
    }

    for(const AActor* Actor : Actors)
    {
        const FVector BloodOriginLocation = Actor->GetActorLocation();
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, OptionalBloodEffect, BloodOriginLocation);
    }
}

void UObsidianDamageGameplayAbility::DamageAllCharactersAndSpawnBloodEffect(const TArray<AActor*>& Actors, UNiagaraSystem* OptionalBloodEffect)
{
    if(Actors.IsEmpty())
    {
        return;
    }
    
    const UAbilitySystemComponent* OwningASC = GetAbilitySystemComponentFromActorInfo();
    const float AbilityLevel = GetAbilityLevel();
    
    FGameplayEffectContextHandle ContextHandle = OwningASC->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	
    const FGameplayEffectSpecHandle SpecHandle = OwningASC->MakeOutgoingSpec(DamageEffectClass, AbilityLevel, ContextHandle);

    for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : BaseDamageTypeMap)
    {
        const float Damage = Pair.Value.RollForDamageNumberAtLevel(AbilityLevel);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Damage);
    }

    if(OptionalBloodEffect == nullptr)
    {
        for(AActor* Actor : Actors)
        {
            if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
            {
                TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            }

            UNiagaraSystem* BloodEffect = IObsidianCombatInterface::Execute_GetBloodEffect(Actor);
            if(BloodEffect == nullptr)
            {
                continue;
            }
        
            const FVector BloodOriginLocation = Actor->GetActorLocation();
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BloodEffect, BloodOriginLocation);
        }
    }
    else
    {
        for(AActor* Actor : Actors)
        {
            if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
            {
                TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            }
            
            const FVector BloodOriginLocation = Actor->GetActorLocation();
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, OptionalBloodEffect, BloodOriginLocation);
        }
    }
}

float FObsidianAbilityDamageRange::RollForDamageNumberAtLevel(const float Level) const
{
    const float MinValue = MinimalDamage.GetValueAtLevel(Level);
    const float MaxValue = MaximalDamage.GetValueAtLevel(Level);
    
    return FMath::FloorToFloat(FMath::RandRange(MinValue, MaxValue));
}



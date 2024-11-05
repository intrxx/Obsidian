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

FVector UObsidianDamageGameplayAbility::PredictActorLocation(AActor* Actor, const float Time, const FVector& FallBackVector)
{
    if(Actor == nullptr)
    {
        return FallBackVector;
    }
    
    const FVector ActorVelocity = Actor->GetVelocity() * FVector(1.0f, 1.0f, 0.0f);
    if(ActorVelocity.IsNearlyZero())
    {
        return Actor->GetActorLocation();
    }
    
    return Actor->GetActorLocation() + (ActorVelocity * Time);
}

FVector UObsidianDamageGameplayAbility::ShortenVector(const FVector& StartVector, const FVector& EndVector,
    const float AmountToShorten)
{
    float ShortenBy = AmountToShorten;
    const FVector OriginalVector = EndVector - StartVector;
    const float OriginalVectorLength = OriginalVector.Length();
    
    if(ShortenBy >= OriginalVectorLength)
    {
        ShortenBy = OriginalVectorLength;
    }

    return StartVector + OriginalVector.GetSafeNormal() * (OriginalVectorLength - ShortenBy);
}

float FObsidianAbilityDamageRange::RollForDamageNumberAtLevel(const float Level) const
{
    const float MinValue = MinimalDamage.GetValueAtLevel(Level);
    const float MaxValue = MaximalDamage.GetValueAtLevel(Level);
    
    return FMath::FloorToFloat(FMath::RandRange(MinValue, MaxValue));
}



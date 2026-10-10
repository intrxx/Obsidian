// Copyright 2026 out of sCope team - intrxx


#include "AbilitySystem/Abilities/ObsidianDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"


UObsidianDamageGameplayAbility::UObsidianDamageGameplayAbility(const FObjectInitializer& InObjectInitializer)
    : Super(InObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UObsidianDamageGameplayAbility::DamageAllCharacters(const TArray<AActor*>& InActorsToDamage)
{
    if(InActorsToDamage.IsEmpty())
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
    
    for(AActor* Actor : InActorsToDamage)
    {
        if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
        {
            TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
}

void UObsidianDamageGameplayAbility::DamageCharacter(AActor* InActorToDamage)
{
    if(!IsValid(InActorToDamage))
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

    if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActorToDamage))
    {
        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

FVector UObsidianDamageGameplayAbility::PredictActorLocation(AActor* InActor, const float InTime, const FVector& InFallBackVector)
{
    if(InActor == nullptr)
    {
        return InFallBackVector;
    }
    
    const FVector ActorVelocity = InActor->GetVelocity() * FVector(1.0f, 1.0f, 0.0f);
    if(ActorVelocity.IsNearlyZero())
    {
        return InActor->GetActorLocation();
    }
    
    return InActor->GetActorLocation() + (ActorVelocity * InTime);
}

FVector UObsidianDamageGameplayAbility::ShortenVector(const FVector& InStartVector, const FVector& InEndVector,
    const float InAmountToShorten)
{
    float ShortenBy = InAmountToShorten;
    const FVector OriginalVector = InEndVector - InStartVector;
    const float OriginalVectorLength = OriginalVector.Length();
    
    if(ShortenBy >= OriginalVectorLength)
    {
        ShortenBy = OriginalVectorLength;
    }

    return InStartVector + OriginalVector.GetSafeNormal() * (OriginalVectorLength - ShortenBy);
}

float FObsidianAbilityDamageRange::RollForDamageNumberAtLevel(const float InLevel) const
{
    const float MinValue = MinimalDamage.GetValueAtLevel(InLevel);
    const float MaxValue = MaximalDamage.GetValueAtLevel(InLevel);
    
    return FMath::FloorToFloat(FMath::RandRange(MinValue, MaxValue));
}



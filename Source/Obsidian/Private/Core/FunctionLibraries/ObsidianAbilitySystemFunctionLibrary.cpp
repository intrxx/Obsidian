// Copyright 2024 out of sCope team - Michał Ogiński

#include "Core/FunctionLibraries/ObsidianAbilitySystemFunctionLibrary.h"

// ~ Core
#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "AbilitySystem/Data/ObsidianEnemyTypeInfo.h"
#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"
#include "Game/ObsidianGameMode.h"

void UObsidianAbilitySystemFunctionLibrary::InitializeEnemyDefaultAttributesWithClass(const UObject* WorldContextObject, UObsidianAbilitySystemComponent* ASC,
                                                                   const EObsidianEnemyClass EnemyClass, const float Level, UObject* SourceObject)
{
    if(WorldContextObject == nullptr)
    {
        return;
    }
	
    if(const AObsidianGameMode* ObsidianGameMode = Cast<AObsidianGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
    {
        FObsidianEnemyTypeDefaultInfo EnemyDefaultInfo = ObsidianGameMode->GetEnemyTypeInfo()->GetEnemyTypeDefaultInfo(EnemyClass);

        if(EnemyDefaultInfo.DefaultAbilitySet)
        {
            EnemyDefaultInfo.DefaultAbilitySet->GiveToAbilitySystem(ASC, nullptr, Level, SourceObject);
        }
    }
}

bool UObsidianAbilitySystemFunctionLibrary::IsBlockedAttack(const FGameplayEffectContextHandle& EffectContextHandle)
{
   if(const FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<const FObsidianGameplayEffectContext*>(EffectContextHandle.Get()))
   {
       return ObsidianEffectContext->IsBlockedAttack();
   }
    return false;
}

bool UObsidianAbilitySystemFunctionLibrary::IsCriticalAttack(const FGameplayEffectContextHandle& EffectContextHandle)
{
    if(const FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<const FObsidianGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return ObsidianEffectContext->IsCriticalAttack();
    }
    return false;
}

void UObsidianAbilitySystemFunctionLibrary::SetIsBlockedAttack(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedAttack)
{
   if(FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<FObsidianGameplayEffectContext*>(EffectContextHandle.Get()))
   {
       ObsidianEffectContext->SetIsBlockedAttack(bInIsBlockedAttack);
   }
}

void UObsidianAbilitySystemFunctionLibrary::SetIsCriticalAttack(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalAttack)
{
    if(FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<FObsidianGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        ObsidianEffectContext->SetIsCriticalAttack(bInIsCriticalAttack);
    }
}

void UObsidianAbilitySystemFunctionLibrary::GetAllOwnedTagsFromActor(AActor* Actor, FGameplayTagContainer& OutTags)
{
    if(const UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
    {
        ASC->GetOwnedGameplayTags(OutTags);
    }
}

void UObsidianAbilitySystemFunctionLibrary::GetAllCharactersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,
                                                                         UClass* ActorClassFilter, const TArray<AActor*>& ActorsToIgnore, const float Radius, const FVector& SphereOrigin, const bool bWithDebug)
{
    FCollisionQueryParams SphereParams;
    SphereParams.AddIgnoredActors(ActorsToIgnore);
    
    if(const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        TArray<FOverlapResult> Overlaps;
        
        World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
            FCollisionShape::MakeSphere(Radius), SphereParams);
        
        for(FOverlapResult& OverlapResult : Overlaps)
        {
            UObsidianAttributesComponent* AttributesComponent =  UObsidianAttributesComponent::FindCommonAttributesComponent(OverlapResult.GetActor());
            if(AttributesComponent && !AttributesComponent->IsDeadOrDying())
            {
                OutOverlappingActors.AddUnique(OverlapResult.GetActor());
            } 
        }

        if(ActorClassFilter != nullptr)
        {
            TArray<AActor*> TempOverlappingActors;
            TempOverlappingActors.Append(OutOverlappingActors);

            OutOverlappingActors.Empty();

            for(AActor* Actor : TempOverlappingActors)
            {
                if(Actor->IsA(ActorClassFilter))
                {
                    OutOverlappingActors.AddUnique(Actor);
                }
            }
        }
        
#if ENABLE_DRAW_DEBUG && !UE_BUILD_SHIPPING
        if(bWithDebug)
        {
            DrawDebugSphere(World, SphereOrigin, Radius, 16, FColor::Blue, false, 5.0f, 0, 1.0f);

            for(AActor* OverlappedActor : OutOverlappingActors)
            {
                DrawDebugSphere(World, OverlappedActor->GetActorLocation(), 16.0f, 8, FColor::Red, false, 5.0f, 0, 1.0f);
            }
        }
#endif
    }
}


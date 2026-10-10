// Copyright 2026 out of sCope team - intrxx

#include "Core/FunctionLibraries/ObsidianAbilitySystemFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"

#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "AbilitySystem/Data/ObsidianEnemyTypeInfo.h"
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"
#include "Game/ObsidianGameMode.h"


void UObsidianAbilitySystemFunctionLibrary::InitializeEnemyDefaultAttributesWithClass(const UObject* InWorldContextObject, UObsidianAbilitySystemComponent* InASC,
                                                                   const EObsidianEnemyClass InEnemyClass, const float InLevel, UObject* InSourceObject)
{
    if(InWorldContextObject == nullptr)
    {
        return;
    }
	
    if(const AObsidianGameMode* ObsidianGameMode = Cast<AObsidianGameMode>(UGameplayStatics::GetGameMode(InWorldContextObject)))
    {
        FObsidianEnemyTypeDefaultInfo EnemyDefaultInfo = ObsidianGameMode->GetEnemyTypeInfo()->GetEnemyTypeDefaultInfo(InEnemyClass);

        if(EnemyDefaultInfo.DefaultAbilitySet)
        {
            EnemyDefaultInfo.DefaultAbilitySet->GiveToAbilitySystem(InASC, nullptr, InLevel, InSourceObject);
        }
    }
}

bool UObsidianAbilitySystemFunctionLibrary::IsBlockedAttack(const FGameplayEffectContextHandle& InEffectContextHandle)
{
   if(const FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<const FObsidianGameplayEffectContext*>(InEffectContextHandle.Get()))
   {
       return ObsidianEffectContext->IsBlockedAttack();
   }
    return false;
}

bool UObsidianAbilitySystemFunctionLibrary::IsCriticalAttack(const FGameplayEffectContextHandle& InEffectContextHandle)
{
    if(const FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<const FObsidianGameplayEffectContext*>(InEffectContextHandle.Get()))
    {
        return ObsidianEffectContext->IsCriticalAttack();
    }
    return false;
}

void UObsidianAbilitySystemFunctionLibrary::SetIsBlockedAttack(FGameplayEffectContextHandle& InOutEffectContextHandle, const bool bInIsBlockedAttack)
{
   if(FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<FObsidianGameplayEffectContext*>(InOutEffectContextHandle.Get()))
   {
       ObsidianEffectContext->SetIsBlockedAttack(bInIsBlockedAttack);
   }
}

void UObsidianAbilitySystemFunctionLibrary::SetIsCriticalAttack(FGameplayEffectContextHandle& InOutEffectContextHandle, const bool bInIsCriticalAttack)
{
    if(FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<FObsidianGameplayEffectContext*>(InOutEffectContextHandle.Get()))
    {
        ObsidianEffectContext->SetIsCriticalAttack(bInIsCriticalAttack);
    }
}

void UObsidianAbilitySystemFunctionLibrary::GetAllOwnedTagsFromActor(AActor* InActor, FGameplayTagContainer& OutTags)
{
    if(const UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor))
    {
        ASC->GetOwnedGameplayTags(OutTags);
    }
}

void UObsidianAbilitySystemFunctionLibrary::GetAllCharactersWithinRadius(const UObject* InWorldContextObject, TArray<AActor*>& OutOverlappingActors,
                                                                         UClass* InActorClassFilter, const TArray<AActor*>& InActorsToIgnore, const float InRadius, const FVector& InSphereOrigin, const bool bInWithDebug)
{
    FCollisionQueryParams SphereParams;
    SphereParams.AddIgnoredActors(InActorsToIgnore);
    
    if(const UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        TArray<FOverlapResult> Overlaps;
        
        World->OverlapMultiByObjectType(Overlaps, InSphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
            FCollisionShape::MakeSphere(InRadius), SphereParams);
        
        for(FOverlapResult& OverlapResult : Overlaps)
        {
            UObsidianAttributesComponent* AttributesComponent =  UObsidianAttributesComponent::FindCommonAttributesComponent(OverlapResult.GetActor());
            if(AttributesComponent && !AttributesComponent->IsDeadOrDying())
            {
                OutOverlappingActors.AddUnique(OverlapResult.GetActor());
            } 
        }

        if(InActorClassFilter != nullptr)
        {
            TArray<AActor*> TempOverlappingActors;
            TempOverlappingActors.Append(OutOverlappingActors);

            OutOverlappingActors.Empty();

            for(AActor* Actor : TempOverlappingActors)
            {
                if(Actor->IsA(InActorClassFilter))
                {
                    OutOverlappingActors.AddUnique(Actor);
                }
            }
        }
        
#if ENABLE_DRAW_DEBUG && !UE_BUILD_SHIPPING
        if(bInWithDebug)
        {
            DrawDebugSphere(World, InSphereOrigin, InRadius, 16, FColor::Blue, false, 5.0f, 0, 1.0f);

            for(AActor* OverlappedActor : OutOverlappingActors)
            {
                DrawDebugSphere(World, OverlappedActor->GetActorLocation(), 16.0f, 8, FColor::Red, false, 5.0f, 0, 1.0f);
            }
        }
#endif
    }
}


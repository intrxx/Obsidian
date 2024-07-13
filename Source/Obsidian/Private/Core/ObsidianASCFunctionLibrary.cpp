// Copyright 2024 out of sCope team - Michał Ogiński


#include "Core/ObsidianASCFunctionLibrary.h"

#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "AbilitySystem/Data/ObsidianEnemyTypeInfo.h"
#include "Game/ObsidianGameMode.h"
#include "Kismet/GameplayStatics.h"

void UObsidianASCFunctionLibrary::InitializeEnemyDefaultAttributesWithClass(const UObject* WorldContextObject, UObsidianAbilitySystemComponent* ASC,
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

bool UObsidianASCFunctionLibrary::IsBlockedAttack(const FGameplayEffectContextHandle& EffectContextHandle)
{
   if(const FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<const FObsidianGameplayEffectContext*>(EffectContextHandle.Get()))
   {
       return ObsidianEffectContext->IsBlockedAttack();
   }
    return false;
}

bool UObsidianASCFunctionLibrary::IsCriticalAttack(const FGameplayEffectContextHandle& EffectContextHandle)
{
    if(const FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<const FObsidianGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return ObsidianEffectContext->IsCriticalAttack();
    }
    return false;
}

void UObsidianASCFunctionLibrary::SetIsBlockedAttack(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedAttack)
{
   if(FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<FObsidianGameplayEffectContext*>(EffectContextHandle.Get()))
   {
       ObsidianEffectContext->SetIsBlockedAttack(bInIsBlockedAttack);
   }
}

void UObsidianASCFunctionLibrary::SetIsCriticalAttack(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalAttack)
{
    if(FObsidianGameplayEffectContext* ObsidianEffectContext = static_cast<FObsidianGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        ObsidianEffectContext->SetIsCriticalAttack(bInIsCriticalAttack);
    }
}

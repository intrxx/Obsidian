// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/ObsidianGameplayAbility_Aura.h"

// ~ Core
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"

// ~ Project

UObsidianGameplayAbility_Aura::UObsidianGameplayAbility_Aura()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
	ActivationPolicy = EObsidianGameplayAbility_ActivationPolicy::EAP_OnInputTriggered;
}

void UObsidianGameplayAbility_Aura::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if(AuraEffectClass)
	{
		// If the AuraEffectHandle is valid, the Aura effect is currently active, and we need to disable it.
		if (AuraEffectHandle.IsValid())
		{
			ObsidianASC = ObsidianASC == nullptr
			 ? TObjectPtr<UObsidianAbilitySystemComponent>(Cast<UObsidianAbilitySystemComponent>(
			 	GetAbilitySystemComponentFromActorInfo()))
				: ObsidianASC;
			
			ObsidianASC->RemoveActiveGameplayEffect(AuraEffectHandle);
			AuraEffectHandle.Invalidate();
			ObsidianASC->OnAuraDisabledDelegate.ExecuteIfBound(EffectUIInfoTag);

			if (ensureMsgf(AuraCostEffectHandle.IsValid(), TEXT("Cost Gameplay Effect Handle is invalid in [%hs]"), __FUNCTION__))
			{
				ObsidianASC->RemoveActiveGameplayEffect(AuraCostEffectHandle);
				AuraCostEffectHandle.Invalidate();
			}
			return;
		}

		if (CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			AuraEffectHandle = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo,
			AuraEffectClass.GetDefaultObject(), GetAbilityLevel());
		}
		else
		{
			const bool bReplicateEndAbility = true;
			const bool bWasCancelled = true;
			EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		}
	}
}

void UObsidianGameplayAbility_Aura::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		const_cast<FActiveGameplayEffectHandle&>(AuraCostEffectHandle) = ApplyGameplayEffectToOwner(Handle, ActorInfo,
			ActivationInfo, CostGE, GetAbilityLevel(Handle, ActorInfo));
	}
}



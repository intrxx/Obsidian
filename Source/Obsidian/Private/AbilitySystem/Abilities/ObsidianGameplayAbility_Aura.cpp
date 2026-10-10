// Copyright 2026 out of sCope team - intrxx


#include "AbilitySystem/Abilities/ObsidianGameplayAbility_Aura.h"

#include "AbilitySystemComponent.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"


UObsidianGameplayAbility_Aura::UObsidianGameplayAbility_Aura()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
	ActivationPolicy = EObsidianGameplayAbility_ActivationPolicy::EAP_OnInputTriggered;
}

void UObsidianGameplayAbility_Aura::ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo,
	const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData)
{
	Super::ActivateAbility(InHandle, InActorInfo, InActivationInfo, InTriggerEventData);
	
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

		if (CommitAbility(InHandle, InActorInfo, InActivationInfo))
		{
			AuraEffectHandle = ApplyGameplayEffectToOwner(InHandle, InActorInfo, InActivationInfo,
			AuraEffectClass.GetDefaultObject(), GetAbilityLevel());
		}
		else
		{
			const bool bReplicateEndAbility = true;
			const bool bWasCancelled = true;
			EndAbility(InHandle, InActorInfo, InActivationInfo, bReplicateEndAbility, bWasCancelled);
		}
	}
}

void UObsidianGameplayAbility_Aura::ApplyCost(const FGameplayAbilitySpecHandle InHandle,
	const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) const
{
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		const_cast<FActiveGameplayEffectHandle&>(AuraCostEffectHandle) = ApplyGameplayEffectToOwner(InHandle, InActorInfo,
			InActivationInfo, CostGE, GetAbilityLevel(InHandle, InActorInfo));
	}
}



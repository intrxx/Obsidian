// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/AI/ObsidianAIGameplayAbility_Melee.h"

void UObsidianAIGameplayAbility_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if(bShouldStopMovement)
	{
		
	}
}

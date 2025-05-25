// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/Abilities/ObsidianGameplayAbility_Death.h"

// ~ Core

// ~ Project
#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"

UObsidianGameplayAbility_Death::UObsidianGameplayAbility_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	
	if(HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag by default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = ObsidianGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UObsidianGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	UObsidianAbilitySystemComponent* ObsidianASC = CastChecked<UObsidianAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	//TODO Can change it to cancel specific abilities or ignore some with specific tag in the future
	ObsidianASC->CancelAllAbilities(this);

	SetCanBeCanceled(false);

	if(bAutoStartDeath)
	{
		StartDeath();
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UObsidianGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	FinishDeath();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UObsidianGameplayAbility_Death::StartDeath()
{
	if(UObsidianAttributesComponent* AttributesComponent = UObsidianAttributesComponent::FindCommonAttributesComponent(GetAvatarActorFromActorInfo()))
	{
		if(AttributesComponent->GetDeathState() == EObsidianDeathState::EDS_Alive)
		{
			AttributesComponent->StartDeath();
		}
	}
}

void UObsidianGameplayAbility_Death::FinishDeath()
{
	if(UObsidianAttributesComponent* AttributesComponent = UObsidianAttributesComponent::FindCommonAttributesComponent(GetAvatarActorFromActorInfo()))
	{
		if(AttributesComponent->GetDeathState() == EObsidianDeathState::EDS_DeathStarted)
		{
			AttributesComponent->FinishDeath();
		}
	}
}

// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/Mass/ObsidianMassStateTreeTask_UseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "MassActorSubsystem.h"
#include "MassCommandBuffer.h"
#include "MassExecutionContext.h"
#include "MassStateTreeExecutionContext.h"
#include "StateTreeLinker.h"

#include "AI/ObsidianEnemyInterface.h"
#include "Mass/ObsidianMassEnemyFragments.h"


FObsidianMassStateTreeTask_UseGameplayAbility::FObsidianMassStateTreeTask_UseGameplayAbility()
{
	bShouldCallTick = false;
}

bool FObsidianMassStateTreeTask_UseGameplayAbility::Link(FStateTreeLinker& InLinker)
{
	InLinker.LinkExternalData(ActorHandle);
	InLinker.LinkExternalData(CombatTargetHandle);
	return true;
}

EStateTreeRunStatus FObsidianMassStateTreeTask_UseGameplayAbility::EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	const FMassStateTreeExecutionContext& MassContext = static_cast<FMassStateTreeExecutionContext&>(InContext);
	const FMassActorFragment& ActorFragment = InContext.GetExternalData(ActorHandle);
	const FObsidianMassCombatTargetFragment& CombatTarget = InContext.GetExternalData(CombatTargetHandle);

	// Abilities live on the Actor, entity that is not hydrated has nothing to activate them on.
	if(ActorFragment.IsValid() == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	const TWeakObjectPtr<AActor> WeakActor = const_cast<AActor*>(ActorFragment.Get());
	const TWeakObjectPtr<AActor> WeakCombatTarget = bSetCombatTargetOnEnemyInterface ? CombatTarget.TargetActor : nullptr;
	const FGameplayTagContainer AbilityTags = ActivateAbilityWithTag;

	MassContext.GetMassEntityExecutionContext().Defer().PushCommand<FMassDeferredSetCommand>(
		[WeakActor, WeakCombatTarget, AbilityTags](FMassEntityManager& InEntityManager)
		{
			AActor* Actor = WeakActor.Get();
			if(Actor == nullptr)
			{
				return;
			}

			if(WeakCombatTarget.IsValid() && Actor->Implements<UObsidianEnemyInterface>())
			{
				IObsidianEnemyInterface::Execute_SetCombatTarget(Actor, WeakCombatTarget.Get());
			}

			if(UAbilitySystemComponent* OwningASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
			{
				OwningASC->TryActivateAbilitiesByTag(AbilityTags, true);
			}
		});

	// The same as it was on the Behavior Tree Task, the Task is done no matter if the Ability was activated.
	return EStateTreeRunStatus::Succeeded;
}

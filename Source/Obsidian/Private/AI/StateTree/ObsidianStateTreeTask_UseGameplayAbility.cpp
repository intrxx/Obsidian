// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/ObsidianStateTreeTask_UseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/Engine.h"
#include "StateTreeExecutionContext.h"

#include "AI/ObsidianEnemyInterface.h"
#include "Obsidian/ObsidianLogCategories.h"


#define LOCTEXT_NAMESPACE "ObsidianStateTree"

FObsidianStateTreeTask_UseGameplayAbility::FObsidianStateTreeTask_UseGameplayAbility()
{
	bShouldCallTick = false;
}

EStateTreeRunStatus FObsidianStateTreeTask_UseGameplayAbility::EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	const FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);

	AActor* Actor = InstanceData.Actor;
	if(Actor == nullptr)
	{
		UE_LOG(ObLogAI, Error, TEXT("FObsidianStateTreeTask_UseGameplayAbility failed since the Actor is missing."));
		return EStateTreeRunStatus::Failed;
	}

	if(bSetCombatTargetOnEnemyInterface)
	{
		if(InstanceData.CombatTarget && Actor->Implements<UObsidianEnemyInterface>())
		{
			IObsidianEnemyInterface::Execute_SetCombatTarget(Actor, InstanceData.CombatTarget);
		}
		else
		{
			UE_LOG(ObLogAI, Error, TEXT("Combat Target is invalid on [FObsidianStateTreeTask_UseGameplayAbility] for [%s]."
				"Make sure to bind the Combat Target on the Task!"), *GetNameSafe(Actor));
		}
	}

	UAbilitySystemComponent* OwningASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	if(OwningASC == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}

	const bool bWasSuccessful = OwningASC->TryActivateAbilitiesByTag(ActivateAbilityWithTag, true);

#if !UE_BUILD_SHIPPING
	if(bDebugEnabled && GEngine)
	{
		const FString Message = bWasSuccessful ? FString::Printf(TEXT("Ability [%s] Activation was successfull."), *ActivateAbilityWithTag.ToString())
			: FString::Printf(TEXT("Ability [%s] Activation failed."), *ActivateAbilityWithTag.ToString());
		const FColor LogColor = bWasSuccessful ? FColor::Green : FColor::Red;

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, LogColor, Message);
	}
#endif

	// The same as it was on the Behavior Tree Task, the Task is done no matter if the Ability was activated.
	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR
FText FObsidianStateTreeTask_UseGameplayAbility::GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView,
	const IStateTreeBindingLookup& InBindingLookup, EStateTreeNodeFormatting InFormatting) const
{
	return FText::Format(LOCTEXT("UseGameplayAbilityDescription", "Use Gameplay Ability {0}"), FText::FromString(ActivateAbilityWithTag.ToStringSimple()));
}
#endif

#undef LOCTEXT_NAMESPACE

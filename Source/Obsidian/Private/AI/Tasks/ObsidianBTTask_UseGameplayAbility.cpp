// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/Tasks/ObsidianBTTask_UseGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "AI/ObsidianEnemyInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Obsidian/Obsidian.h"

UObsidianBTTask_UseGameplayAbility::UObsidianBTTask_UseGameplayAbility()
{
	NodeName = FString("Use Gameplay Ability");
	INIT_TASK_NODE_NOTIFY_FLAGS();
}


EBTNodeResult::Type UObsidianBTTask_UseGameplayAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if(AIController == nullptr)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UObsidianBTTask_UseGameplayAbility::ExecuteTask failed since AIController is missing."));
		NodeResult = EBTNodeResult::Failed;
	}
	else
	{
		NodeResult = PerformUseGameplayAbilityTask(OwnerComp, NodeMemory);
	}
	
	return NodeResult;
}

EBTNodeResult::Type UObsidianBTTask_UseGameplayAbility::PerformUseGameplayAbilityTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	AActor* Actor = AIController->GetPawn();

	
	if(bSetCombatTargetOnEnemyInterface)
	{
		const UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if(AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(CombatTarget_Selector.SelectedKeyName)))
		{
			IObsidianEnemyInterface::Execute_SetCombatTarget(Actor, TargetActor);
		}
		else
		{
			UE_LOG(LogObsidian, Error, TEXT("Target Actor Blackboard Key Selector is invalid on [UObsidianBTTask_UseGameplayAbility] for [%s]."
								   "Make sure to set the TargetActor on the Node!"), *GetNameSafe(AIController));
		}
	}
	
	if(Actor)
	{
		if(UAbilitySystemComponent* OwningASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			OwningASC->TryActivateAbilitiesByTag(ActivateAbilityWithTag, true);
			NodeResult = EBTNodeResult::Succeeded;
		}
	}
	
	return NodeResult;
}

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

	bool bWasSuccessful = false;
	if(Actor)
	{
		if(UAbilitySystemComponent* OwningASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			
			bWasSuccessful = OwningASC->TryActivateAbilitiesByTag(ActivateAbilityWithTag, true);
			NodeResult = EBTNodeResult::Succeeded;
		}
	}

#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		const FString Message = bWasSuccessful ? FString::Printf(TEXT("Ability [%s] Activation was successfull."), *ActivateAbilityWithTag.ToString())
				: FString::Printf(TEXT("Ability [%s] Activation failed."), *ActivateAbilityWithTag.ToString());
		const FColor LogColor = bWasSuccessful ? FColor::Green : FColor::Red;
		
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, LogColor, Message);
	}
#endif
	
	return NodeResult;
}

FString UObsidianBTTask_UseGameplayAbility::GetStaticDescription() const
{
	FString BasicMessage = FString::Printf(TEXT("Gameplay Tag: [%s] \n"), *ActivateAbilityWithTag.ToString()) +=
		bSetCombatTargetOnEnemyInterface ? FString::Printf(TEXT("Set Target on Enemy Interface. \n")) : FString::Printf(TEXT("Do not set Target on Enemy Interface. \n"));

	if(bSetCombatTargetOnEnemyInterface)
	{
		BasicMessage += FString::Printf(TEXT("Combat Target: [%s] \n"), *CombatTarget_Selector.SelectedKeyName.ToString());
	}
	
	BasicMessage += bDebugEnabled ? FString::Printf(TEXT("Debug Enabled.")) : FString::Printf(TEXT("Debug Disabled."));
		
	return BasicMessage;
}

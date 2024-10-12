// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/Tasks/ObsidianBTTask_SetFocus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UObsidianBTTask_SetFocus::UObsidianBTTask_SetFocus()
{
	NodeName = FString("Set Focus");
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UObsidianBTTask_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if(AIController && BlackboardComponent)
	{
		if(AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(Target_Selector.SelectedKeyName)))
		{
			AIController->SetFocus(Target);
			
			NodeResult = EBTNodeResult::Succeeded;
		}
		else
		{
			UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UObsidianBTTask_SetFocus::ExecuteTask failed since Target to Focus is missing."));
			NodeResult = EBTNodeResult::Failed;
		}
	}
	else
	{
		UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UObsidianBTTask_SetFocus::ExecuteTask failed since AIController is missing."));
		NodeResult = EBTNodeResult::Failed;
	}
	
	return NodeResult;
}

FString UObsidianBTTask_SetFocus::GetStaticDescription() const
{
	return FString::Printf(TEXT("Sets Focus on Target's AI Controller. \n")) +=
		FString::Printf(TEXT("Target Key: [%s] \n"), *Target_Selector.SelectedKeyName.ToString());
}
